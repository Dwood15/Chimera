#include "scope_fix.h"
#include "../hac2/hac2.h"
#include "../messaging/messaging.h"
#include "../client_signature.h"
#include "../hooks/tick.h"

Resolution &get_resolution() noexcept {
    static Resolution *resolution = *reinterpret_cast<Resolution **>(get_signature("resolution_sig").address() + 4);
    return *resolution;
}

static void apply_fix() {
    static float *width = reinterpret_cast<float *>(get_signature("widescreen_scope_sig").address() + 4);
    static float *height = reinterpret_cast<float *>(get_signature("widescreen_scope_sig").address() + 4 + 8);
    auto &resolution = get_resolution();
    float ar = static_cast<float>(resolution.width) / resolution.height;
    auto expected = ar * *height;
    if(expected != *width) {
        DWORD prota;
        DWORD protb;
        VirtualProtect(width, sizeof(*width), PAGE_READWRITE, &prota);
        *width = expected;
        VirtualProtect(width, sizeof(*width), prota, &protb);
    }
}

ChimeraCommandError widescreen_scope_fix_command(size_t argc, const char **argv) noexcept {
    static bool widescreen_scope_fix = false;
    if(argc == 1) {
        auto &widescreen_scope_sig = get_signature("widescreen_scope_sig");
        bool new_value = bool_value(argv[0]);
        if(new_value != widescreen_scope_fix) {
            if(new_value) {
                if(!hac2_present())
                    console_out_warning("chimera_widescreen_scope_mask: HAC2 is not installed, so this function does nothing.");
                else
                    add_tick_event(apply_fix);
            }
            else {
                remove_tick_event(apply_fix);
                widescreen_scope_sig.undo();
            }
            widescreen_scope_fix = new_value;
        }
    }
    char x[256] = {};
    sprintf(x, "chimera_widescreen_scope_mask: %s", widescreen_scope_fix ? "true" : "false");
    console_out(x);
    return CHIMERA_COMMAND_ERROR_SUCCESS;
}
