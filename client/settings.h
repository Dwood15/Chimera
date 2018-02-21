#pragma once

#include "command/command.h"

struct ChimeraStartupParameters {
    char fast_startup = 0;
    char keystone = 0;
    char unused[0x1FE] = {};
};
static_assert(sizeof(ChimeraStartupParameters) == 0x200);

ChimeraStartupParameters &startup_parameters() noexcept;
void execute_startup_parameters() noexcept;

void commit_command(const char *command, size_t argc, const char **argv) noexcept;
void save_all_changes() noexcept;

bool read_init_file(const char *path, const char *name) noexcept;
ChimeraCommandError verbose_init_command(size_t argc, const char **argv) noexcept;
