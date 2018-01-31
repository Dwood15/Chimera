#pragma once

#include "../command/command.h"

/// Function for command chimera_block_vsync
ChimeraCommandError block_vsync_command(size_t argc, const char **argv) noexcept;

/// Function for command chimera_set_resolution
ChimeraCommandError set_resolution_command(size_t argc, const char **argv) noexcept;
