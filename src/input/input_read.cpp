// LOCATION: /home/user/ACE/src/input/input_read.cpp
#include "input/input_read.h"
#include <SDL3/SDL.h>
#include <iostream>
#include <unistd.h>
#include <limits.h>

InputReader::InputReader(SDL_JoystickID id) : instance_id(id) {
    gamepad = SDL_OpenGamepad(id);
    if (gamepad) {
        state.name = SDL_GetGamepadName(gamepad);
        const char* devPath = SDL_GetGamepadPath(gamepad); 
        if (devPath) {
            std::string p = devPath;
            size_t h = p.find("hidraw");
            if (h != std::string::npos) {
                char rp[PATH_MAX];
                if (realpath(("/sys/class/hidraw/" + p.substr(h)).c_str(), rp)) {
                    std::string full = rp;
                    size_t u = full.find("/usb");
                    if (u != std::string::npos) {
                        size_t col = full.find(':', u + 4);
                        state.path = full.substr(u + 5, col - u - 5);
                    }
                }
            }
        }
        SDL_SetGamepadSensorEnabled(gamepad, SDL_SENSOR_ACCEL, true);
        SDL_SetGamepadSensorEnabled(gamepad, SDL_SENSOR_GYRO, true);
        state.connected = true;
        std::cout << "\033[1;32m[INPUT] Found: " << state.name << " on " << state.path << "\033[0m" << std::endl;
    }
}

void InputReader::update() {
    if (!gamepad) return;
    for (int i = 0; i < SDL_GAMEPAD_AXIS_COUNT; i++) 
        state.axes[i] = SDL_GetGamepadAxis(gamepad, (SDL_GamepadAxis)i);
    for (int i = 0; i < SDL_GAMEPAD_BUTTON_COUNT; i++) 
        state.buttons[i] = SDL_GetGamepadButton(gamepad, (SDL_GamepadButton)i);

    // Now matching the float* requirements of SDL3
    SDL_GetGamepadSensorData(gamepad, SDL_SENSOR_GYRO, state.gyro, 3);
    SDL_GetGamepadSensorData(gamepad, SDL_SENSOR_ACCEL, state.accel, 3);
}

InputReader::~InputReader() { if(gamepad) SDL_CloseGamepad(gamepad); }
