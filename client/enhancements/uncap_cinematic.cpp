#include "../client_signature.h"
#include "../messaging/messaging.h"
#include "uncap_cinematic.h"

ChimeraCommandError uncap_cinematic_command(size_t argc, const char **argv) noexcept {
    static bool uncap_cinematic = false;
    if(argc == 1) {
        auto &uncap_cinematic_s = get_signature("uncap_cinematic_sig");
        bool new_value = bool_value(argv[0]);
        if(new_value != uncap_cinematic) {
            if(new_value) {
                const short uncap_cinematic_mod[] = {0xEB, 0x04, 0xB3, 0x01, 0xEB, 0x02, 0x32, 0xDB, 0x8B, 0x2D};
                write_code_s(uncap_cinematic_s.address(),uncap_cinematic_mod);
            }
            else {
                uncap_cinematic_s.undo();
                execute_chimera_command("chimera_throttle_fps 0", true);
            }
            uncap_cinematic = new_value;
        }
    }
    char x[256] = {};
    sprintf(x, "chimera_uncap_cinematic: %s", uncap_cinematic ? "true" : "false");
    console_out(x);
    return CHIMERA_COMMAND_ERROR_SUCCESS;
}
