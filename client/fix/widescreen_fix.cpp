#include "widescreen_fix.h"

#include <string.h>
#include "../hac2/scope_fix.h"
#include "../hud_mod/offset_hud_elements.h"
#include "../messaging/messaging.h"
#include "../hooks/map_load.h"
#include "../hooks/tick.h"
#include "../client_signature.h"

static char *objects;
static std::vector<SafeZoneMod> mods;

float width_scale = 0;

static void set_mod(bool force = false);
extern void apply_scope_fix();
extern void undo_scope_fix();

static void on_map_load() {
    set_mod(true);
}

static void apply_offsets() {
    set_mod();
    offset_tick(objects, mods, 320.0 - 320.0 * width_scale, 0);
}

static void set_mod(bool force) {
    static float adder = 1.0;
    static float adder_negative = -1.0;
    auto &resolution = get_resolution();
    float aspect_ratio = static_cast<float>(resolution.width) / resolution.height;
    float new_width_scale = aspect_ratio / (4.0 / 3.0);
    if(width_scale != new_width_scale || force) {
        width_scale = new_width_scale;
        float p_scale = 1.0/(320.0 * new_width_scale);
        adder = 1.0 / new_width_scale;
        adder_negative = -(1.0 + 1.0 / 640.0) / new_width_scale;

        auto *hud_element_widescreen_sig_address = get_signature("hud_element_widescreen_sig").address();
        write_code_any_value(hud_element_widescreen_sig_address + 0x7, p_scale);
        write_code_any_value(hud_element_widescreen_sig_address + 0xC0 + 2, &adder);

        auto *hud_element_motion_sensor_blip_widescreen_address = get_signature("hud_element_motion_sensor_blip_widescreen_sig").address();
        write_code_any_value(hud_element_motion_sensor_blip_widescreen_address + 0x4, p_scale);
        write_code_any_value(hud_element_motion_sensor_blip_widescreen_address + 0x1B + 3 + 4, adder_negative);

        auto *hud_text_widescreen_sig = get_signature("hud_text_widescreen_sig").address();
        write_code_any_value(hud_text_widescreen_sig + 2 + 4, p_scale);
        write_code_any_value(hud_text_widescreen_sig + 0x1E + 2 + 4, adder_negative);

        offset_undo(mods);
        offset_map_load(objects, mods, 320.0 - 320.0 * width_scale, 0, false);
        apply_scope_fix();
    }
}

bool widescreen_fix_active = false;

ChimeraCommandError widescreen_fix_command(size_t argc, const char **argv) noexcept {
    extern bool widescreen_scope_mask_active;
    if(argc == 1) {
        bool new_value = bool_value(argv[0]);
        if(new_value != widescreen_fix_active) {
            if(new_value) {
                objects = new char[65535]();
                if(widescreen_scope_mask_active) {
                    execute_chimera_command("chimera_widescreen_scope_mask 0", true);
                }
                add_tick_event(apply_offsets);
                add_map_load_event(on_map_load);
                on_map_load();
            }
            else {
                delete[] objects;
                offset_undo(mods);
                remove_tick_event(apply_offsets);
                remove_map_load_event(on_map_load);
                get_signature("hud_element_widescreen_sig").undo();
                get_signature("hud_element_motion_sensor_blip_widescreen_sig").undo();
                get_signature("hud_text_widescreen_sig").undo();
                undo_scope_fix();
                width_scale = 0;
            }
            widescreen_fix_active = new_value;
        }
    }
    console_out(std::string("chimera_widescreen_fix: ") + (widescreen_fix_active ? "true" : "false"));
    return CHIMERA_COMMAND_ERROR_SUCCESS;
}
