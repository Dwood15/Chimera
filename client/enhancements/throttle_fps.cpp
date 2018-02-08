#include "throttle_fps.h"
#include <sys/time.h>
#include "../client_signature.h"
#include "../hooks/frame.h"
#include "../messaging/messaging.h"

static struct timespec prev_time;
static double throttle_fps = 0.0;

static void after_frame() noexcept {
    auto seconds_per_frame = 1.0 / throttle_fps;
    do {
        struct timespec now_time;
        clock_gettime(CLOCK_MONOTONIC,&now_time);
        auto r = static_cast<double>(now_time.tv_sec - prev_time.tv_sec) + static_cast<double>(now_time.tv_nsec - prev_time.tv_nsec) / 1000000000.0;
        if(r >= seconds_per_frame) {
            prev_time = now_time;
            break;
        }
        Sleep(0);
    }
    while(true);
}

ChimeraCommandError throttle_fps_command(size_t argc, const char **argv) noexcept {
    if(argc == 1) {
        auto new_value = atof(argv[0]);
        if(new_value != throttle_fps) {
            if(new_value <= 5.0) {
                if(new_value != 0.0) {
                    console_out_error("chimera_throttle_fps: Invalid framerate.");
                    return CHIMERA_COMMAND_ERROR_FAILURE;
                }
                new_value = 0.0;
            }

            if(new_value == 0.0) {
                remove_frame_event(after_frame);
            }
            else {
                add_frame_event(after_frame, EVENT_PRIORITY_FINAL);
                clock_gettime(CLOCK_MONOTONIC,&prev_time);
            }
            throttle_fps = new_value;
        }
    }
    if(throttle_fps == 0.0) {
        console_out("chimera_throttle_fps: off");
    }
    else {
        console_out(std::string("chimera_throttle_fps: ") + std::to_string(throttle_fps));
    }
    return CHIMERA_COMMAND_ERROR_SUCCESS;
}
