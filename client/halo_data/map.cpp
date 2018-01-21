#include "map.h"

#include "../client_signature.h"

MapHeader &get_map_header() {
    static auto *map_header = *reinterpret_cast<MapHeader **>(get_signature("map_header_sig").address() + 2);
    return *map_header;
}
