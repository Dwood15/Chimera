#include <sys/time.h>

#include "../client_signature.h"
#include "../command/command.h"
#include "../messaging/messaging.h"
#include "tick.h"

static struct timespec current_tick_time;

void on_map_load() noexcept;
static void initialize_tick() noexcept;
static bool tick_initialized = false;

static std::vector<Event<event_no_args>> preevents;

void add_pretick_event(event_no_args event_function, EventPriority priority) noexcept {
    for(size_t i=0;i<preevents.size();i++) {
        if(preevents[i].function == event_function) return;
    }

    if(!tick_initialized) initialize_tick();
    preevents.emplace_back(event_function, priority);
}
void remove_pretick_event(event_no_args event_function) noexcept {
    for(size_t i=0;i<preevents.size();i++) {
        if(preevents[i].function == event_function) {
            preevents.erase(preevents.begin() + i);
            return;
        }
    }
}

static void on_pretick() noexcept {
    if(tick_count() == 0) {
        on_map_load();
    }
    call_in_order(preevents);
}

static std::vector<Event<event_no_args>> events;

void add_tick_event(event_no_args event_function, EventPriority priority) noexcept {
    for(size_t i=0;i<events.size();i++) {
        if(events[i].function == event_function) return;
    }
    if(!tick_initialized) initialize_tick();
    events.emplace_back(event_function, priority);
}

void remove_tick_event(event_no_args event_function) noexcept {
    for(size_t i=0;i<events.size();i++) {
        if(events[i].function == event_function) {
            events.erase(events.begin() + i);
            return;
        }
    }
}

static void on_tick() noexcept {
    clock_gettime(CLOCK_MONOTONIC, &current_tick_time);
    call_in_order(events);
}

static void initialize_tick() noexcept {
    tick_initialized = true;
    static BasicCodecave on_tick_bytecode;
    write_jmp_call(get_signature("on_tick_sig").address(), reinterpret_cast<void *>(on_pretick), reinterpret_cast<void *>(on_tick), on_tick_bytecode);
}

int32_t tick_count() noexcept {
    static auto *tick_counter = reinterpret_cast<int32_t *>(**reinterpret_cast<char ***>(get_signature("tick_counter_sig").address() + 1) + 0xC);
    return *tick_counter;
}

const float &tick_rate() noexcept {
    static auto *trs = *reinterpret_cast<float **>(get_signature("tick_rate_sig").address() + 2);
    return *trs;
}

void set_tick_rate(float new_rate) noexcept {
    static auto &trs = *const_cast<float *>(&tick_rate());
    DWORD prota, protb;
    VirtualProtect(&trs, sizeof(trs), PAGE_READWRITE, &prota);
    trs = new_rate;
    VirtualProtect(&trs, sizeof(trs), prota, &protb);
    return;
}

float effective_tick_rate() noexcept {
    static auto *address = get_signature("game_speed_sig").address();
    return *reinterpret_cast<float *>(**reinterpret_cast<char ***>(address + 1) + 0x18) * tick_rate();
}

double tick_time() noexcept {
    struct timespec now_time;
    clock_gettime(CLOCK_MONOTONIC,&now_time);
    auto r = static_cast<double>(now_time.tv_sec - current_tick_time.tv_sec) + static_cast<double>(now_time.tv_nsec - current_tick_time.tv_nsec) / 1000000000.0;
    return r;
}

double tick_progress() noexcept {
    auto current_time = tick_time() * effective_tick_rate();
    if(current_time < 1.0) return current_time;
    else return 1.0;
}

double tick_progress_inaccurate() noexcept {
    auto current_time = tick_time() * effective_tick_rate();
    if(current_time < 1.666666667) return current_time;
    else return 1.666666667;
}
