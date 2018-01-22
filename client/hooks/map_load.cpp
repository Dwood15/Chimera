#include "map_load.h"
#include <vector>
#include <stdint.h>

#include "tick.h"
#include "../client_signature.h"

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
