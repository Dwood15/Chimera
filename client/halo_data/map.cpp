#include "map.h"

#include "../client_signature.h"

MapHeader &get_map_header() noexcept {
    static auto *map_header = *reinterpret_cast<MapHeader **>(get_signature("map_header_sig").address() + 2);
    return *map_header;
}

MapIndex *map_indices() noexcept {
    static auto **all_map_indices = *reinterpret_cast<MapIndex ***>(get_signature("map_index_sig").address() + 2);
    return *all_map_indices;
}

uint32_t maps_count() noexcept {
    static auto *map_count = *reinterpret_cast<uint32_t **>(get_signature("map_index_sig").address() + 2) + 1;
    return *map_count;
}
