// LOCATION: /home/user/ACE/include/logic/logic.h
#ifndef LOGIC_H
#define LOGIC_H

#include <cstdint>
#include <atomic>
#include "vehicles/plane/input_tune_plane.h"

class Logic {
public:
    bool initialize();
    void run_iteration();
    static void unlock_backend() { backend_unlocked.store(true); }

    const int16_t* getRawAxes(int slot) const;
    const bool* getButtons(int slot) const;
    const float* getAccel() const;
    const float* getGyro() const;

private:
    InputTunePlane plane_processor;
    int last_known_mode = -1;
    static std::atomic<bool> backend_unlocked; 
};
#endif
