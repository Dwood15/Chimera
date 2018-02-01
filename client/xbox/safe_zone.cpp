#include "safe_zone.h"
#include <string.h>
#include "../halo_data/table.h"
#include "../halo_data/tag_data.h"
#include "../messaging/messaging.h"
#include "../hooks/map_load.h"
#include "../hooks/tick.h"

static char *objects;

struct SafeZoneMod {
    short *address;
    short delta;
};
static std::vector<SafeZoneMod> mods;
static void change_xy(void *addr) {
    auto *x = reinterpret_cast<short *>(addr) + 0;
    auto *y = reinterpret_cast<short *>(addr) + 1;
    SafeZoneMod x_mod;
    x_mod.address = x;
    mods.push_back(x_mod);
    SafeZoneMod y_mod;
    y_mod.address = y;
    mods.push_back(y_mod);
    x_mod.delta = 33;
    y_mod.delta = 25;
    *x += 33;
    *y += 25;
}

#define not_center(addr) *reinterpret_cast<short *>(addr) != 4

static void do_things_to_wphi_tag(HaloTagID tag_id) {
    if(tag_id.is_valid() && !objects[tag_id.index]) {
        objects[tag_id.index] = true;
        auto *&tdata = HaloTag::from_id(tag_id).data;
        do_things_to_wphi_tag(*reinterpret_cast<HaloTagID *>(tdata + 0xC));
        if(not_center(tdata + 0x3C)) {
            char *static_elements = *reinterpret_cast<char **>(tdata + 0x60 + 4);
            for(int k=0;k<*reinterpret_cast<uint32_t *>(tdata + 0x60);k++) {
                char *static_element = static_elements + k * 180;
                change_xy(static_element + 0x24);
            }
            char *meter_elements = *reinterpret_cast<char **>(tdata + 0x6C + 4);
            for(int k=0;k<*reinterpret_cast<uint32_t *>(tdata + 0x6C);k++) {
                char *meter_element = meter_elements + k * 180;
                change_xy(meter_element + 0x24);
            }
            char *number_elements = *reinterpret_cast<char **>(tdata + 0x78 + 4);
            for(int k=0;k<*reinterpret_cast<uint32_t *>(tdata + 0x78);k++) {
                char *number_element = number_elements + k * 160;
                change_xy(number_element + 0x24);
            }
            char *overlay_elements = *reinterpret_cast<char **>(tdata + 0x90 + 4);
            for(int o=0;o<*reinterpret_cast<uint32_t *>(tdata + 0x90);o++) {
                char *overlay_element = overlay_elements + o * 104;
                char *overlays = *reinterpret_cast<char **>(overlay_element + 0x34 + 4);
                for(int k=0;k<*reinterpret_cast<uint32_t *>(overlay_element + 0x34);k++) {
                    char *overlay = overlays + k * 136;
                    change_xy(overlay + 0x0);
                }
            }
        }
    }
}

static void on_map_load() {
    memset(objects, 0, 65535);
    mods.clear();
    for(size_t i=0;i<*reinterpret_cast<uint32_t *>(0x4044000C);i++) {
        auto &tag = HaloTag::from_id(i);
        auto *&tdata = tag.data;
        objects[i] = true;
        if(tag.tag_class == 0x68756467) {
            if(not_center(tdata + 0x0)) {
                change_xy(tdata + 0x24);
            }
        }
        else if(tag.tag_class == 0x6D617467) {
            auto *&data = *reinterpret_cast<char **>(tdata + 0x128 + 4);
            for(size_t g=0;g<*reinterpret_cast<uint32_t *>(tdata + 0x128);g++) {
                auto &tag_id = *reinterpret_cast<HaloTagID *>(data + g * 68 + 0x14 + 0xC);
                if(tag_id.is_valid() && !objects[tag_id.index]) {
                    objects[tag_id.index] = true;
                    auto &tag = HaloTag::from_id(tag_id);
                    auto *&tdata = tag.data;
                    if(not_center(tdata + 0x0)) {
                        change_xy(tdata + 0x24);
                        change_xy(tdata + 0x8C);
                        change_xy(tdata + 0xF4);
                        char *overlays = *reinterpret_cast<char **>(tdata + 0x15C + 4);
                        for(int k=0;k<*reinterpret_cast<uint32_t *>(tdata + 0x15C);k++) {
                            char *overlay = overlays + k * 136;
                            change_xy(overlay);
                        }
                    }
                }
            }
        }
        else {
            if(tag.tag_class == 0x6269746D) {
                const char *satanic_1 = "ui\\shell\\bitmaps\\team_icon";
                const auto satanic_1_size = strlen(satanic_1);
                const char *satanic_2 = "ui\\shell\\bitmaps\\team_background";
                const auto satanic_2_size = strlen(satanic_2);
                if(memcmp(tag.path,satanic_1,satanic_1_size) == 0 || memcmp(tag.path,satanic_2,satanic_2_size) == 0) {
                    auto *addr = reinterpret_cast<short *>(const_cast<char *>(tag.path));
                    SafeZoneMod x_mod;
                    x_mod.address = addr;
                    x_mod.delta = 1;
                    (*addr)++;
                    mods.push_back(x_mod);
                }
            }

            objects[i] = false;
        }
    }
}

static void apply_safe_zones() {
    HaloPlayer player;
    HaloObject object(player.object_id());
    auto *object_data = object.object_data();
    if(object_data) {
        auto &tag_id = *reinterpret_cast<HaloTagID *>(object_data);
        if(!objects[tag_id.index]) {
            objects[tag_id.index] = true;
            auto &tag = HaloTag::from_id(tag_id);
            auto *&tag_data = tag.data;
            auto *&data_ptr = *reinterpret_cast<char **>(tag_data + 0x2A8 + 4);
            for(auto h=0;h<*reinterpret_cast<uint32_t *>(tag_data + 0x2A8);h++) {
                auto &tag_id = *reinterpret_cast<HaloTagID *>(data_ptr + h * 48 + 0xC);
                if(tag_id.is_valid() && !objects[tag_id.index]) {
                    objects[tag_id.index] = true;
                    auto &unhi_tag = HaloTag::from_id(tag_id);
                    auto *&tdata = unhi_tag.data;
                    if(not_center(tdata + 0x0)) {
                        change_xy(tdata + 0x24);
                        change_xy(tdata + 0x8C);
                        change_xy(tdata + 0xF4);
                        change_xy(tdata + 0x17C);
                        change_xy(tdata + 0x1E4);
                        change_xy(tdata + 0x26C);
                        change_xy(tdata + 0x2D4);
                        change_xy(tdata + 0x35C);
                        char *aux_overlays = *reinterpret_cast<char **>(tdata + 0x3A4 + 4);
                        for(int k=0;k<*reinterpret_cast<uint32_t *>(tdata + 0x3A4);k++) {
                            char *aux_overlay = aux_overlays + k * 132;
                            change_xy(aux_overlay);
                        }
                        char *aux_meters = *reinterpret_cast<char **>(tdata + 0x3CC + 4);
                        for(int k=0;k<*reinterpret_cast<uint32_t *>(tdata + 0x3CC);k++) {
                            char *aux_meter = aux_meters + k * 324;
                            change_xy(aux_meter + 0x14);
                            change_xy(aux_meter + 0x7C);
                        }
                    }
                }
            }
        }
        auto &weapon = *reinterpret_cast<uint32_t *>(object_data + 0x118);
        auto object = HaloObject(weapon);
        auto *object_data = object.object_data();
        if(object_data && *reinterpret_cast<uint16_t *>(object_data + 0xB4) == 2) {
            auto &weapon_tag_id = *reinterpret_cast<HaloTagID *>(object_data);
            if(!objects[weapon_tag_id.index]) {
                objects[weapon_tag_id.index] = true;
                do_things_to_wphi_tag(*reinterpret_cast<HaloTagID *>(HaloTag::from_id(weapon_tag_id).data + 0x480 + 0xC));
            }
        }
    }
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
                for(size_t i=0;i<mods.size();i++) {
                    *mods[i].address -= mods[i].delta;
                }
                mods.clear();
                remove_tick_event(apply_safe_zones);
                remove_map_load_event(on_map_load);
            }
            active = new_value;
        }
    }
    console_out(std::string("chimera_safe_zones: ") + (active ? "true" : "false"));
    return CHIMERA_COMMAND_ERROR_SUCCESS;
}
