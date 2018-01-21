#pragma once

#include "../command/command.h"

struct Resolution {
    uint16_t height;
    uint16_t width;
};

/// Obtain a reference to the current resolution of Halo.
Resolution &get_resolution() noexcept;

/// Function for command chimera_widescreen_scope_fix
ChimeraCommandError widescreen_scope_fix_command(size_t argc, const char **argv) noexcept;
