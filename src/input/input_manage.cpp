// LOCATION: /home/user/ACE/src/input/input_manage.cpp
#include "input/input_manage.h"
#include <SDL3/SDL.h>
#include <iostream>
#include <iterator>

InputManager& InputManager::instance() { static InputManager inst; return inst; }
InputManager::InputManager() : current_vehicle_type(-1) {}
InputManager::~InputManager() { SDL_Quit(); }

bool InputManager::initialize() {
    if (!SDL_Init(SDL_INIT_GAMEPAD | SDL_INIT_SENSOR | SDL_INIT_JOYSTICK)) return false;
    scoutDevices();
    return true;
}

void InputManager::setVehicleType(int type) {
    if (current_vehicle_type == type) return; // Fix for the double-log issue
    current_vehicle_type = type;
    std::cout << "\033[1;36m[MANAGER] Vehicle Mode Confirmed: " << type << "\033[0m" << std::endl;
}

void InputManager::scoutDevices() {
    int count;
    SDL_JoystickID* joysticks = SDL_GetJoysticks(&count);
    
    auto it = m_devices.begin();
    while (it != m_devices.end()) {
        bool still_there = false;
        for (int i = 0; i < count; i++) if (it->first == joysticks[i]) still_there = true;
        if (!still_there) {
            std::cout << "\033[1;31m[SAFETY] Device lost. Wiping slot data.\033[0m" << std::endl;
            it = m_devices.erase(it);
        } else ++it;
    }

    if (joysticks) {
        for (int i = 0; i < count; i++) {
            if (m_devices.find(joysticks[i]) == m_devices.end()) {
                m_devices[joysticks[i]] = std::make_unique<InputReader>(joysticks[i]);
            }
        }
        SDL_free(joysticks);
    }
}

void InputManager::update() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_GAMEPAD_ADDED || event.type == SDL_EVENT_GAMEPAD_REMOVED) {
            scoutDevices();
        }
    }
    for (auto const& [id, dev] : m_devices) dev->update();
}

DeviceState InputManager::getSlotState(int index) const {
    if (index >= 0 && index < (int)m_devices.size()) {
        auto it = m_devices.begin();
        std::advance(it, index);
        return it->second->getState();
    }
    return DeviceState(); 
}
