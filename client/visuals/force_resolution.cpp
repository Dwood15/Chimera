#include "force_resolution.h"

#include <stdlib.h>
#include "../messaging/messaging.h"
#include "../client_signature.h"

struct ChangeResolutionStructA {
    uint32_t width;
    uint32_t height;
    uint32_t unknown1 = 0x3C;
    uint32_t unknown2 = 0;
};

struct ChangeResolutionStructB {
    uint32_t width;
    uint32_t height;
    uint32_t unknown1 = 16;
    uint32_t unknown2 = 1;

    uint32_t unknown3 = 0;
    uint32_t unknown4 = 0;
    uint32_t unknown5 = 3;
    uint32_t unknown6 = 0;

    uint32_t windowed = 1;
    uint32_t no_acid_trip_plz = 1;
    uint32_t unknown7 = 0x4B;
    uint32_t unknown8 = 0;

    uint32_t unknown9 = 0;
    /// 0x80000000 = no vsync; 0 = vsync
    uint32_t no_vsync = 0x80000000;
};

static_assert(sizeof(ChangeResolutionStructB) == 0x38);

void set_resolution(int width, int height) {
    auto *change_resolution_query = get_signature("change_resolution_query_sig").address();
    auto *change_resolution = get_signature("change_resolution_sig").address();
    auto *change_window_size = get_signature("change_window_size_sig").address();

    ChangeResolutionStructA z;
    z.height = height;
    z.width = width;
    ChangeResolutionStructB r;

    asm (
        "pushad;"
        "push %0;"
        "mov eax, %1;"
        "call %2;"
        "add esp, 4;"
        "popad;"
        :
        : "r" (&r), "r" (&z), "m" (change_resolution_query)
    );
    r.width = width;
    r.height = height;
    asm (
        "pushad;"
        "push %0;"
        "call %1;"
        "add esp, 4;"
        "popad;"
        :
        : "r" (&r), "m" (change_resolution)
    );
    asm (
        "pushad;"
        "mov eax, %0;"
        "mov ecx, %1;"
        "mov esi, 1;"
        "call %2;"
        "popad;"
        :
        : "m" (height), "m" (width), "m" (change_window_size)
    );
}

/// Function for command chimera_force_resolution
ChimeraCommandError force_resolution_command(size_t argc, const char **argv) noexcept {
    int width = strtol(argv[0], nullptr, 10);
    int height = strtol(argv[1], nullptr, 10);
    if(width < 640 || height < 480) {
        console_out_error("chimera_force_resolution requires a resolution of at least 640 x 480");
        return CHIMERA_COMMAND_ERROR_FAILURE;
    }
    set_resolution(width, height);
    return CHIMERA_COMMAND_ERROR_SUCCESS;
}
