#pragma once

#include "../command/command.h"

/// Function for command chimera_block_update_notifier
ChimeraCommandError block_update_notifier_command(size_t argc, const char **argv);

/// Check for updates.
void check_updater();
