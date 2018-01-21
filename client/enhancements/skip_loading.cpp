#include "skip_loading.h"

#include "../messaging/messaging.h"
#include "../client_signature.h"

ChimeraCommandError skip_loading_command(size_t argc, const char **argv) noexcept {
    static auto enabled = false;
    if(argc == 1) {
        bool new_value = bool_value(argv[0]);
        if(new_value != enabled) {
            auto &loading_screen_1_s = get_signature("loading_screen_1_sig");
            auto &loading_screen_2_s = get_signature("loading_screen_2_sig");
            auto &loading_screen_3_s = get_signature("loading_screen_3_sig");
            if(new_value) {
                const short loading_screen_mod[]   = {  -1,   -1, -1, -1, -1, -1, 0x00, 0x00, 0x00, 0x00};
                write_code_s(loading_screen_1_s.address(), loading_screen_mod);
                write_code_s(loading_screen_2_s.address(), loading_screen_mod);
                write_code_s(loading_screen_3_s.address(), loading_screen_mod);
            }
            else {
                loading_screen_1_s.undo();
                loading_screen_2_s.undo();
                loading_screen_3_s.undo();
            }
            enabled = new_value;
        }
    }
    char x[256] = {};
    sprintf(x, "chimera_skip_loading: %s", enabled ? "true" : "false");
    console_out(x);
    return CHIMERA_COMMAND_ERROR_SUCCESS;
}
