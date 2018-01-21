#pragma once

#include "event.h"

void add_map_load_event(event_no_args event_function, EventPriority priority = EVENT_PRIORITY_DEFAULT) noexcept;
void remove_map_load_event(event_no_args event_function) noexcept;

void add_map_preload_event(event_no_args event_function, EventPriority priority = EVENT_PRIORITY_DEFAULT) noexcept;
void remove_map_preload_event(event_no_args event_function) noexcept;
