// LOCATION: /home/user/ACE/include/input/input_manage.h
#ifndef INPUT_MANAGE_H
#define INPUT_MANAGE_H

#include <SDL3/SDL.h>
#include <memory>
#include <map>
#include "input_read.h"

class InputManager {
public:
    static InputManager& instance();
    bool initialize();
    void update();
    void setVehicleType(int type);
    int getVehicleType() const { return current_vehicle_type; }
    DeviceState getSlotState(int index) const;
    ~InputManager(); // Added declaration

private:
    InputManager();
    void scoutDevices(); // Added declaration
    
    std::map<SDL_JoystickID, std::unique_ptr<InputReader>> m_devices;
    int current_vehicle_type;
};
#endif
