#pragma once

#include "command.h"

/// Initialize the console code.
void initialize_console();

/// Return true if the user has the console open.
bool console_is_out();

/// Function for command chimera_enable_console
ChimeraCommandError enable_console_command(size_t argc, const char **argv) noexcept;
