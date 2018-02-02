#include "safe_zone.h"
#include <string.h>
#include "../halo_data/hud.h"
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

static void change_xy(AnchorOffset &addr) {
    auto *x = &addr.x;
    auto *y = &addr.y;
    SafeZoneMod x_mod;
    x_mod.address = x;
    x_mod.delta = 33;
    mods.push_back(x_mod);
    SafeZoneMod y_mod;
    y_mod.address = y;
    y_mod.delta = 25;
    mods.push_back(y_mod);
    *x += 33;
    *y += 25;
}
static void change_xy(HUDElementPosition &addr) {
    change_xy(addr.anchor_offset);
}

#define not_center(addr) *reinterpret_cast<short *>(addr) != 4

static void do_things_to_wphi_tag(HaloTagID tag_id) {
    if(tag_id.is_valid() && !objects[tag_id.index]) {
        objects[tag_id.index] = true;
        auto &tdata = *reinterpret_cast<WeaponHUDInterface *>(HaloTag::from_id(tag_id).data);
        do_things_to_wphi_tag(tdata.child.tag_id);
        if(tdata.anchor != ANCHOR_CENTER) {
            for(int i=0;i<tdata.static_elements_count;i++) {
                change_xy(tdata.static_elements[i].prelude.position);
            }
            for(int i=0;i<tdata.meter_elements_count;i++) {
                change_xy(tdata.meter_elements[i].prelude.position);
            }
            for(int i=0;i<tdata.number_elements_count;i++) {
                change_xy(tdata.number_elements[i].prelude.position);
            }
            for(int o=0;o<tdata.overlay_elements_count;o++) {
                auto &overlay = tdata.overlay_elements[o];
                for(int i=0;i<overlay.overlays_count;i++) {
                    change_xy(overlay.overlays[i].position);
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
                change_xy(*reinterpret_cast<AnchorOffset *>(tdata + 0x24));
            }
        }
        else if(tag.tag_class == 0x6D617467) {
            auto *&grenades = *reinterpret_cast<char **>(tdata + 0x128 + 4);
            for(size_t g=0;g<*reinterpret_cast<uint32_t *>(tdata + 0x128);g++) {
                auto &tag_id = *reinterpret_cast<HaloTagID *>(grenades + g * 68 + 0x14 + 0xC);
                if(tag_id.is_valid() && !objects[tag_id.index]) {
                    objects[tag_id.index] = true;
                    auto &tag = HaloTag::from_id(tag_id);
                    GrenadeHUDInterface &tdata = *reinterpret_cast<GrenadeHUDInterface *>(tag.data);
                    if(tdata.anchor != ANCHOR_CENTER) {
                        change_xy(tdata.grenade_hud_background.position);
                        change_xy(tdata.total_grenades_background.position);
                        change_xy(tdata.total_grenades_numbers.position);
                        for(uint32_t o=0;o<tdata.overlays_count;o++) {
                            change_xy(tdata.overlays[o].position);
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
                    auto &tdata = *reinterpret_cast<UnitHUDInterface *>(unhi_tag.data);
                    if(tdata.anchor != ANCHOR_CENTER) {
                        change_xy(tdata.unit_hud_background.position);
                        change_xy(tdata.shield_panel_background.position);
                        change_xy(tdata.shield_panel_meter.prelude.position);
                        change_xy(tdata.health_panel_background.position);
                        change_xy(tdata.health_panel_meter.prelude.position);
                        change_xy(tdata.motion_sensor_background.position);
                        change_xy(tdata.motion_sensor_foreground.position);
                        change_xy(tdata.motion_sensor_center);
                        for(uint32_t i=0;i<tdata.auxiliary_hud_meters_count;i++) {
                            change_xy(tdata.auxiliary_hud_meters[i].background.position);
                        }
                    }
                    if(tdata.auxiliary_overlays.anchor != ANCHOR_CENTER) {
                        for(uint32_t i=0;i<tdata.auxiliary_overlays.overlays_count;i++) {
                            change_xy(tdata.auxiliary_overlays.overlays[i].position);
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
