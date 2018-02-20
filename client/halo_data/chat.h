#pragma once

#include "../command/command.h"

void chat_out(const char *message, uint32_t channel) noexcept;

/// Function for command chimera_chat
ChimeraCommandError chat_command(size_t argc, const char **argv) noexcept;
