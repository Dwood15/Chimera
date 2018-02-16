#include "mouse_acceleration.h"
#include "../client_signature.h"
#include "../messaging/messaging.h"

ChimeraCommandError block_mouse_acceleration_command(size_t argc, const char **argv) noexcept {
    static auto active = false;
    if(argc == 1) {
        bool new_value = bool_value(argv[0]);
        if(new_value != active) {
            auto &mouse_accel_1_sig = get_signature("mouse_accel_1_sig");
            auto &mouse_accel_2_sig = get_signature("mouse_accel_2_sig");
            if(new_value) {
                write_code_any_value(mouse_accel_1_sig.address(), static_cast<unsigned char>(0xEB));
                write_code_any_value(mouse_accel_2_sig.address(), static_cast<unsigned short>(0x9090));
            }
            else {
                mouse_accel_1_sig.undo();
                mouse_accel_2_sig.undo();
            }
            active = new_value;
        }
    }
    console_out(active ? "true" : "false");
    return CHIMERA_COMMAND_ERROR_SUCCESS;
}
