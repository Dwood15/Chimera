#pragma once

#include "command/command.h"

void commit_command(const char *command, size_t argc, const char **argv) noexcept;
void save_all_changes() noexcept;

bool read_init_file(const char *path, const char *name) noexcept;
ChimeraCommandError verbose_init_command(size_t argc, const char **argv) noexcept;
