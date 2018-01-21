#pragma once

#include "event.h"

typedef bool (*event_rcon_message)(const char *);

void add_event_rcon_message(event_rcon_message event_function, EventPriority priority = EVENT_PRIORITY_DEFAULT) noexcept;
void remove_event_rcon_message(event_rcon_message event_function) noexcept;
void initialize_rcon_message() noexcept;
