#include "map_load.h"
#include <vector>
#include <stdint.h>

#include "tick.h"
#include "../client_signature.h"

static bool map_load_initialized = false;

static std::vector<Event<event_no_args>> events;

void add_map_load_event(event_no_args event_function, EventPriority priority) noexcept {
    for(size_t i=0;i<events.size();i++) {
        if(events[i].function == event_function) return;
    }
    events.emplace_back(event_function, priority);
}

void remove_map_load_event(event_no_args event_function) noexcept {
    for(size_t i=0;i<events.size();i++) {
        if(events[i].function == event_function) {
            events.erase(events.begin() + i);
            return;
        }
    }
}

void on_map_load() noexcept {
    call_in_order(events);
}

static void initialize_map_preload() noexcept;
static bool map_preload_initialized = false;

static std::vector<Event<event_no_args>> preevents;

void add_map_preload_event(event_no_args event_function, EventPriority priority) noexcept {
    for(size_t i=0;i<preevents.size();i++) {
        if(preevents[i].function == event_function) return;
    }
    if(!map_preload_initialized) initialize_map_preload();
    preevents.emplace_back(event_function, priority);
}

void remove_map_preload_event(event_no_args event_function) noexcept {
    for(size_t i=0;i<preevents.size();i++) {
        if(preevents[i].function == event_function) {
            preevents.erase(preevents.begin() + i);
            return;
        }
    }
}

static void on_map_preload() noexcept {
    call_in_order(preevents);
}

static void initialize_map_preload() noexcept {
    map_preload_initialized = true;
    static BasicCodecave map_load_code;
    write_jmp_call(get_signature("on_map_preload_sig").address(), reinterpret_cast<void *>(on_map_preload), nullptr, map_load_code);
}
