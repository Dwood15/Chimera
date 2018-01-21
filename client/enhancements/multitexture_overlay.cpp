#include "multitexture_overlay.h"
#include "../client_signature.h"
#include "../messaging/messaging.h"

ChimeraCommandError block_mo_command(size_t argc, const char **argv) noexcept {
    static auto enabled = false;
    if(argc == 1) {
        bool new_value = bool_value(argv[0]);
        if(new_value != enabled) {
            auto &multitexture_overlay_sig = get_signature("multitexture_overlay_sig");
            if(new_value) {
                const short multitexture_overlay_mod[] = {  -1,   -1, 0x60};
                write_code_s(multitexture_overlay_sig.address(), multitexture_overlay_mod);
            }
            else {
                multitexture_overlay_sig.undo();
            }
            enabled = new_value;
        }
    }
    char x[256] = {};
    sprintf(x, "chimera_block_mo: %s", enabled ? "true" : "false");
    console_out(x);
    return CHIMERA_COMMAND_ERROR_SUCCESS;
}
