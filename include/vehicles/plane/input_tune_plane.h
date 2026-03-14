#ifndef INPUT_TUNE_PLANE_H
#define INPUT_TUNE_PLANE_H

#include <cstdint>
#include "input/device_state.h"

struct PlaneCommands {
    int16_t roll = 0;
    int16_t pitch = 0;
    int16_t yaw = 0;
    int16_t throttle = 0;
};

class InputTunePlane {
public:
    InputTunePlane();
    void update();
    PlaneCommands process(const DeviceState& raw);
    int16_t apply_deadzone_smooth(int16_t raw, float dz_percent);
    int16_t apply_expo_raw(int16_t raw, float expo_percent);
};

#endif
