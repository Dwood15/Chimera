#pragma once

#include "../hooks/event.h"
#include "../halo_data/hud.h"

#define OFFSETTER_INDEX_NULL 0
typedef size_t OffsetterIndex;

/// Create an offsetter and return its index.
OffsetterIndex create_offsetter(short x, short y, bool text, EventPriority priority = EVENT_PRIORITY_DEFAULT) noexcept;

/// Delete an offsetter.
void destroy_offsetter(OffsetterIndex index);

//void offset_map_load(char *objects, std::vector<SafeZoneMod> &mods, short x, short y, bool text) noexcept;
//void offset_tick(char *objects, std::vector<SafeZoneMod> &mods, short x, short y) noexcept;
//void offset_undo(std::vector<SafeZoneMod> &mods) noexcept;
