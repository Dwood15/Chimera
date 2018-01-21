#include "anisotropic_filtering.h"

#include "../client_signature.h"
#include "../messaging/messaging.h"

ChimeraCommandError af_command(size_t argc, const char **argv) noexcept {
    static bool active = false;
    static int default_setting = -1;
    if(argc == 1) {
        auto new_value = bool_value(argv[0]);
        if(new_value != active) {
            auto &setting = **reinterpret_cast<char **>(get_signature("af_is_enabled_sig").address() + 1);
            if(default_setting == -1) {
                if(setting) {
                    console_out_warning("chimera_af: Anisotropic filtering is already enabled (likely via config.txt)!");
                    default_setting = 1;
                }
                else {
                    default_setting = 0;
                }
            }
            if(default_setting == 0) setting = new_value;
            active = new_value;
        }
    }
    char x[256] = {};
    sprintf(x, "chimera_af: %s", active ? "true" : "false");
    console_out(x);
    return CHIMERA_COMMAND_ERROR_SUCCESS;
}
