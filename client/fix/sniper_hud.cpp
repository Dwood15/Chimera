#include "sniper_hud.h"
#include "../../version.h"

#include "../messaging/messaging.h"
#include "../hooks/map_load.h"
#include "../halo_data/tag_data.h"
#include "../halo_data/hud.h"

static void sniper_fix() noexcept {
    try {
        auto &tag = HaloTag::lookup("wphi", "weapons\\sniper rifle\\sniper rifle");
        WeaponHUDInterface &hud_interface = *reinterpret_cast<WeaponHUDInterface *>(tag.data);
        #define assert_or_bail(v) if(!(v)) { console_out_warning("Error " STR(v)); return; }
        assert_or_bail(hud_interface.anchor == ANCHOR_TOP_LEFT);

        #define STATIC_ELEMENTS_COUNT 3
        assert_or_bail(hud_interface.static_elements_count == STATIC_ELEMENTS_COUNT);
        assert_or_bail(hud_interface.static_elements[0].multitexture_overlay_count == 0);

        auto &first_item = hud_interface.static_elements[1];
        auto &second_item = hud_interface.static_elements[2];

        assert_or_bail(first_item.multitexture_overlay_count == 1);
        assert_or_bail(second_item.multitexture_overlay_count == 1);

        assert_or_bail(first_item.prelude.position.anchor_offset.x == 92);
        assert_or_bail(first_item.prelude.position.anchor_offset.y == 85);
        assert_or_bail(second_item.prelude.position.anchor_offset.x == 445);
        assert_or_bail(second_item.prelude.position.anchor_offset.y == 85);

        assert_or_bail(first_item.multitexture_overlays->blend_function == HUDMultitextureOverlay::FRAMEBUFFER_BLEND_ALPHA_BLEND);
        assert_or_bail(second_item.multitexture_overlays->blend_function == HUDMultitextureOverlay::FRAMEBUFFER_BLEND_ALPHA_BLEND);

        assert_or_bail(first_item.multitexture_overlays->blending_function_0_to_1 == MULTITEXTURE_OVERLAY_BLENDING_FUNCTION_MULTIPLY);
        assert_or_bail(second_item.multitexture_overlays->blending_function_0_to_1 == MULTITEXTURE_OVERLAY_BLENDING_FUNCTION_MULTIPLY);

        assert_or_bail(first_item.multitexture_overlays->blending_function_1_to_2 == MULTITEXTURE_OVERLAY_BLENDING_FUNCTION_ADD);
        assert_or_bail(second_item.multitexture_overlays->blending_function_1_to_2 == MULTITEXTURE_OVERLAY_BLENDING_FUNCTION_ADD);

        first_item.multitexture_overlays->blending_function_0_to_1 = MULTITEXTURE_OVERLAY_BLENDING_FUNCTION_SUBTRACT;
        second_item.multitexture_overlays->blending_function_0_to_1 = MULTITEXTURE_OVERLAY_BLENDING_FUNCTION_SUBTRACT;

        first_item.prelude.position.anchor_offset.x = 132;
        first_item.prelude.position.anchor_offset.y = 124;
        first_item.prelude.position.height_scale = 0.89;

        second_item.prelude.position.anchor_offset.x = 484;
        second_item.prelude.position.anchor_offset.y = 124;
        second_item.prelude.position.height_scale = 0.89;

        first_item.colors.default_color = ColorByte(1.0F, 0.75F, 0.55F, 0.95F);
        second_item.colors.default_color = first_item.colors.default_color;
    }
    catch(...) {
        console_out_warning("No tag.");
    }
}

ChimeraCommandError sniper_hud_fix_command(size_t argc, const char **argv) noexcept {
    static bool active = false;
    if(argc == 1) {
        bool new_value = bool_value(argv[0]);
        if(new_value != active) {
            if(new_value) {
                add_map_load_event(sniper_fix, EVENT_PRIORITY_BEFORE);
            }
            else {
                remove_map_load_event(sniper_fix);
            }
            active = new_value;
        }
    }
    console_out(active ? "true" : "false");
    return CHIMERA_COMMAND_ERROR_SUCCESS;
}
