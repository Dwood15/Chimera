#pragma once

#include "command/command.h"

void setup_keystone_override() noexcept;

// Function for chimera_keystone command
ChimeraCommandError keystone_command(size_t argc, const char **argv) noexcept;
