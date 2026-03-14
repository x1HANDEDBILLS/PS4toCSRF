#ifndef INPUT_READ_H
#define INPUT_READ_H

#include <SDL3/SDL.h>
#include <string>
#include <vector>
#include "input/device_state.h" // Point to the primary definition

class InputReader {
public:
    InputReader(SDL_JoystickID id);
    ~InputReader();
    void update();
    bool isOpen() const { return gamepad != nullptr; }
    SDL_JoystickID getInstanceId() const { return instance_id; }
    DeviceState getState() const { return state; }

private:
    SDL_Gamepad* gamepad = nullptr;
    SDL_JoystickID instance_id;
    DeviceState state;
};

#endif
