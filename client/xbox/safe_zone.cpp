#include "safe_zone.h"
#include <string.h>
#include "../hud_mod/offset_hud_elements.h"
#include "../messaging/messaging.h"
#include "../hooks/map_load.h"
#include "../hooks/tick.h"

static char *objects;
static std::vector<SafeZoneMod> mods;

static void mod_team_background() {
    for(size_t i=0;i<*reinterpret_cast<uint32_t *>(0x4044000C);i++) {
        auto &tag = HaloTag::from_id(i);
        if(tag.tag_class == 0x6269746D) {
            const char *satanic = "ui\\shell\\bitmaps\\team_background";
            const auto satanic_size = strlen(satanic);
            if(memcmp(tag.path,satanic,satanic_size) == 0) {
                tag.path++;
            }
            else if(memcmp(tag.path-1,satanic,satanic_size) == 0) {
                tag.path--;
            }
        }
    }
}

static void on_map_load() {
    offset_map_load(objects, mods, 33, 25, true);
    mod_team_background();
}

static void apply_safe_zones() {
    offset_tick(objects, mods, 33, 25);
}

ChimeraCommandError safe_zones_command(size_t argc, const char **argv) noexcept {
    static auto active = false;
    if(argc == 1) {
        bool new_value = bool_value(argv[0]);
        if(new_value != active) {
            if(new_value) {
                objects = new char[65535]();
                add_tick_event(apply_safe_zones);
                add_map_load_event(on_map_load);
                on_map_load();
            }
            else {
                delete[] objects;
                offset_undo(mods);
                remove_tick_event(apply_safe_zones);
                remove_map_load_event(on_map_load);
                mod_team_background();
            }
            active = new_value;
        }
    }
    console_out(std::string("chimera_safe_zones: ") + (active ? "true" : "false"));
    return CHIMERA_COMMAND_ERROR_SUCCESS;
}
