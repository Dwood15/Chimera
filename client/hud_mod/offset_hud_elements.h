#pragma once

#include <vector>

#include "../halo_data/hud.h"

struct SafeZoneMod {
    AnchorOffset *address;
    AnchorOffset delta;
};

void offset_map_load(char *objects, std::vector<SafeZoneMod> &mods, short x, short y, bool text) noexcept;
void offset_tick(char *objects, std::vector<SafeZoneMod> &mods, short x, short y) noexcept;
void offset_undo(std::vector<SafeZoneMod> &mods) noexcept;
