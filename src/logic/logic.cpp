// LOCATION: /home/user/ACE/src/logic/logic.cpp
#include "logic/logic.h"
#include "input/input_manage.h"
#include <iostream>

std::atomic<bool> Logic::backend_unlocked{false};
static DeviceState persistent_slots[4];

bool Logic::initialize() {
    last_known_mode = -1;
    return true; 
}

void Logic::run_iteration() {
    if (!backend_unlocked.load()) return;

    int current_mode = InputManager::instance().getVehicleType();
    if (current_mode < 0) return;

    if (current_mode != last_known_mode) {
        std::cout << "\033[1;32m[BACKEND] Engaged Pipeline: " << current_mode << "\033[0m" << std::endl;
        last_known_mode = current_mode;
    }

    DeviceState pilot_raw = InputManager::instance().getSlotState(0);
    if (current_mode == 0 && pilot_raw.connected) {
        plane_processor.process(pilot_raw);
    }
}

const int16_t* Logic::getRawAxes(int slot) const {
    if (slot < 0 || slot > 3) return nullptr;
    persistent_slots[slot] = InputManager::instance().getSlotState(slot);
    return persistent_slots[slot].axes;
}
const bool* Logic::getButtons(int slot) const {
    if (slot < 0 || slot > 3) return nullptr;
    persistent_slots[slot] = InputManager::instance().getSlotState(slot);
    return persistent_slots[slot].buttons;
}
const float* Logic::getAccel() const {
    DeviceState check = InputManager::instance().getSlotState(0);
    persistent_slots[0] = check;
    return check.connected ? persistent_slots[0].accel : nullptr;
}
const float* Logic::getGyro() const {
    DeviceState check = InputManager::instance().getSlotState(0);
    persistent_slots[0] = check;
    return check.connected ? persistent_slots[0].gyro : nullptr;
}
