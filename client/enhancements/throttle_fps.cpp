#include "throttle_fps.h"
#include "../client_signature.h"
#include "../hooks/tick.h"
#include "../messaging/messaging.h"

static void fps_throttle() {
    static auto *do_throttle_fps = *reinterpret_cast<char **>(get_signature("do_throttle_fps_sig").address() + 1);
    *do_throttle_fps = 1;
}

ChimeraCommandError throttle_fps_command(size_t argc, const char **argv) noexcept {
    static double throttle_fps = 0.0;
    if(argc == 1) {
        auto &fps_throttle_s = get_signature("fps_throttle_sig");
        auto new_value = atof(argv[0]);
        if(new_value != throttle_fps) {
            if(new_value <= 5.0) {
                if(new_value != 0.0) {
                    console_out_error("chimera_throttle_fps: Invalid framerate.");
                    return CHIMERA_COMMAND_ERROR_FAILURE;
                }
                new_value = 0.0;
            }
            auto *frametime = *reinterpret_cast<double **>(fps_throttle_s.address() + 2);
            DWORD prota;
            DWORD protb;
            VirtualProtect(reinterpret_cast<char *>(frametime),sizeof(*frametime),PAGE_READWRITE,&prota);
            if(new_value == 0.0) {
                *frametime = 1.0/30.0;
                **reinterpret_cast<char **>(get_signature("do_throttle_fps_sig").address() + 1) = 0;
                remove_tick_event(fps_throttle);
            }
            else {
                *frametime = 1.0/new_value;
                if(throttle_fps == 0) add_tick_event(fps_throttle);
                execute_chimera_command("chimera_uncap_cinematic 1", true);
            }
            VirtualProtect(reinterpret_cast<char *>(frametime),sizeof(*frametime),prota,&protb);
            throttle_fps = new_value;
        }
    }
    if(throttle_fps == 0.0) {
        console_out("chimera_throttle_fps: off");
    }
    else {
        char x[256] = {};
        sprintf(x, "chimera_throttle_fps: %f", throttle_fps);
        console_out(x);
    }
    return CHIMERA_COMMAND_ERROR_SUCCESS;
}
