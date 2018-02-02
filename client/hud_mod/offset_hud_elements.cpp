#include "offset_hud_elements.h"
#include "../halo_data/table.h"

#include <string.h>

static void change_xy(AnchorOffset &addr, std::vector<SafeZoneMod> &mods, short x, short y) {
    SafeZoneMod mod;
    mod.address = &addr;
    mod.delta = {x, y};
    addr.x += x;
    addr.y += y;
    mods.push_back(mod);
}
static void change_xy(HUDElementPosition &addr, std::vector<SafeZoneMod> &mods, short x, short y) {
    change_xy(addr.anchor_offset, mods, x, y);
}

#define not_center(addr) *reinterpret_cast<short *>(addr) != 4

static void do_things_to_wphi_tag(HaloTagID tag_id, char *objects, std::vector<SafeZoneMod> &mods, short x, short y) noexcept {
    if(tag_id.is_valid() && !objects[tag_id.index]) {
        objects[tag_id.index] = true;
        auto &tdata = *reinterpret_cast<WeaponHUDInterface *>(HaloTag::from_id(tag_id).data);
        do_things_to_wphi_tag(tdata.child.tag_id, objects, mods, x, y);
        if(tdata.anchor != ANCHOR_CENTER) {
            for(int i=0;i<tdata.static_elements_count;i++) {
                change_xy(tdata.static_elements[i].prelude.position, mods, x, y);
            }
            for(int i=0;i<tdata.meter_elements_count;i++) {
                change_xy(tdata.meter_elements[i].prelude.position, mods, x, y);
            }
            for(int i=0;i<tdata.number_elements_count;i++) {
                change_xy(tdata.number_elements[i].prelude.position, mods, x, y);
            }
            for(int o=0;o<tdata.overlay_elements_count;o++) {
                auto &overlay = tdata.overlay_elements[o];
                for(int i=0;i<overlay.overlays_count;i++) {
                    change_xy(overlay.overlays[i].position, mods, x, y);
                }
            }
        }
    }
}

void offset_map_load(char *objects, std::vector<SafeZoneMod> &mods, short x, short y, bool text) noexcept {
    memset(objects, 0, 65535);
    mods.clear();
    for(size_t i=0;i<*reinterpret_cast<uint32_t *>(0x4044000C);i++) {
        auto &tag = HaloTag::from_id(i);
        auto *&tdata = tag.data;
        objects[i] = true;
        if(text && tag.tag_class == 0x68756467) {
            if(not_center(tdata + 0x0)) {
                change_xy(*reinterpret_cast<AnchorOffset *>(tdata + 0x24), mods, x, y);
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
                        change_xy(tdata.grenade_hud_background.position, mods, x, y);
                        change_xy(tdata.total_grenades_background.position, mods, x, y);
                        change_xy(tdata.total_grenades_numbers.position, mods, x, y);
                        for(uint32_t o=0;o<tdata.overlays_count;o++) {
                            change_xy(tdata.overlays[o].position, mods, x, y);
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
                    change_xy(*reinterpret_cast<AnchorOffset *>(const_cast<char *>(tag.path)), mods, x, y);
                }
            }

            objects[i] = false;
        }
    }
}

static void do_things_to_unhi_tag(HaloTagID tag_id, char *objects, std::vector<SafeZoneMod> &mods, short x, short y) noexcept {
    if(tag_id.is_valid() && !objects[tag_id.index]) {
        objects[tag_id.index] = true;
        auto &unhi_tag = HaloTag::from_id(tag_id);
        auto &tdata = *reinterpret_cast<UnitHUDInterface *>(unhi_tag.data);
        if(tdata.anchor != ANCHOR_CENTER) {
            change_xy(tdata.unit_hud_background.position, mods, x, y);
            change_xy(tdata.shield_panel_background.position, mods, x, y);
            change_xy(tdata.shield_panel_meter.prelude.position, mods, x, y);
            change_xy(tdata.health_panel_background.position, mods, x, y);
            change_xy(tdata.health_panel_meter.prelude.position, mods, x, y);
            change_xy(tdata.motion_sensor_background.position, mods, x, y);
            change_xy(tdata.motion_sensor_foreground.position, mods, x, y);
            change_xy(tdata.motion_sensor_center, mods, x, y);
            for(uint32_t i=0;i<tdata.auxiliary_hud_meters_count;i++) {
                change_xy(tdata.auxiliary_hud_meters[i].background.position, mods, x, y);
                change_xy(tdata.auxiliary_hud_meters[i].meter.prelude.position, mods, x, y);
            }
        }
        if(tdata.auxiliary_overlays.anchor != ANCHOR_CENTER) {
            for(uint32_t i=0;i<tdata.auxiliary_overlays.overlays_count;i++) {
                change_xy(tdata.auxiliary_overlays.overlays[i].position, mods, x, y);
            }
        }
    }
}

static void do_things_to_weap_tag(HaloTagID tag_id, char *objects, std::vector<SafeZoneMod> &mods, short x, short y) noexcept {
    if(!objects[tag_id.index]) {
        objects[tag_id.index] = true;
        do_things_to_wphi_tag(*reinterpret_cast<HaloTagID *>(HaloTag::from_id(tag_id).data + 0x480 + 0xC), objects, mods, x, y);
    }
}

static void do_things_to_unit_tag(HaloTagID tag_id, char *objects, std::vector<SafeZoneMod> &mods, short x, short y) noexcept {
    if(tag_id.is_valid() && !objects[tag_id.index]) {
        objects[tag_id.index] = true;
        auto &tag = HaloTag::from_id(tag_id);
        auto *&tag_data = tag.data;
        auto *&data_ptr = *reinterpret_cast<char **>(tag_data + 0x2A8 + 4);
        for(uint32_t h=0;h<*reinterpret_cast<uint32_t *>(tag_data + 0x2A8);h++) {
            auto &tag_id = *reinterpret_cast<HaloTagID *>(data_ptr + h * 48 + 0xC);
            do_things_to_unhi_tag(tag_id, objects, mods, x, y);
        }

        for(uint32_t w=0;w<*reinterpret_cast<uint32_t *>(tag_data + 0x2D8);w++) {
            auto *weapon = *reinterpret_cast<char **>(tag_data + 0x2D8 + 4) + w * 36;
            do_things_to_weap_tag(*reinterpret_cast<HaloTagID *>(weapon + 0xC),objects,mods,x,y);
        }

        for(uint32_t s=0;s<*reinterpret_cast<uint32_t *>(tag_data + 0x2E4);s++) {
            auto *seat = *reinterpret_cast<char **>(tag_data + 0x2E4 + 4) + s * 284;
            auto *&hud_ptr = *reinterpret_cast<char **>(seat + 0xDC + 4);
            for(uint32_t h=0;h<*reinterpret_cast<uint32_t *>(seat + 0xDC);h++) {
                do_things_to_unhi_tag(*reinterpret_cast<HaloTagID *>(hud_ptr + h * 48 + 0xC), objects, mods, x, y);
            }
        }
    }
}

void offset_tick(char *objects, std::vector<SafeZoneMod> &mods, short x, short y) noexcept {
    HaloPlayer player;
    HaloObject object(player.object_id());
    auto *object_data = object.object_data();
    if(object_data) {
        auto &odata = *reinterpret_cast<BaseHaloObject *>(object_data);
        if(odata.object_type > 1) return;
        do_things_to_unit_tag(odata.tag_id, objects, mods, x, y);

        HaloObject vehicle_object(odata.parent_object_id);
        auto *vobject_data = vehicle_object.object_data();
        if(vobject_data) {
            auto &vdata = *reinterpret_cast<BaseHaloObject *>(vobject_data);
            if(vdata.object_type <= 1) {
                do_things_to_unit_tag(vdata.tag_id, objects, mods, x, y);
            }
        }

        auto weapon_object = HaloObject(odata.weapon_object_id);
        auto *weapon_data = weapon_object.object_data();
        if(weapon_data && *reinterpret_cast<uint16_t *>(weapon_data + 0xB4) == 2) {
            do_things_to_weap_tag(*reinterpret_cast<HaloTagID *>(weapon_data),objects,mods,x,y);
        }
    }
}

void offset_undo(std::vector<SafeZoneMod> &mods) noexcept {
    for(size_t i=0;i<mods.size();i++) {
        mods[i].address->x -= mods[i].delta.x;
        mods[i].address->y -= mods[i].delta.y;
    }
    mods.clear();
}
