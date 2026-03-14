#include "vehicles/plane/input_tune_plane.h"
#include "Core/app_state.h"
#include <cmath>
#include <algorithm>

InputTunePlane::InputTunePlane() {}

int16_t InputTunePlane::apply_deadzone_smooth(int16_t raw, float dz_percent) {
    int32_t threshold = static_cast<int32_t>(32768.0f * (dz_percent / 100.0f));
    int32_t abs_raw = std::abs(raw);
    if (abs_raw <= threshold) return 0;
    float norm = (abs_raw - threshold) / (32768.0f - (float)threshold);
    return (raw > 0) ? static_cast<int16_t>(norm * 32767.0f) : static_cast<int16_t>(-norm * 32767.0f);
}

int16_t InputTunePlane::apply_expo_raw(int16_t raw, float expo_percent) {
    double x = raw / 32768.0;
    double e = expo_percent / 100.0;
    // Cubic expo formula
    double curve = e * (x * x * x) + (1.0 - e) * x;
    return static_cast<int16_t>(curve * 32767.0);
}

PlaneCommands InputTunePlane::process(const DeviceState& raw) {
    auto& state = AppState::instance();
    PlaneCommands cmd;
    
    // Process primary flight controls with Deadzone + Expo
    int16_t processed[3];
    for(int i=0; i<3; ++i) {
        processed[i] = apply_deadzone_smooth(raw.axes[i], state.deadzones[i]);
        if(state.expo_enabled[i]) {
            processed[i] = apply_expo_raw(processed[i], state.current_expo);
        }
    }

    cmd.roll     = processed[0];
    cmd.pitch    = processed[1];
    cmd.yaw      = processed[2];
    cmd.throttle = raw.axes[3]; // Throttle usually direct
    
    return cmd;
}
