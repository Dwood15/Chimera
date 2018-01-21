#pragma once

#include <string>
#include <vector>

#include "lua/lua.hpp"
#include "../hooks/event.h"
#include "../messaging/messaging.h"

struct LuaScriptCallback {
    std::string callback_function;
    EventPriority priority;
};

struct LuaScriptTimer {
    double interval_ms;
    double time_passed = 0;
    std::string function;
    size_t timer_id;
    std::vector<std::string> arguments;
};

struct LuaScript {
    lua_State *state = nullptr;

    std::vector<LuaScriptTimer> timers;

    std::string name;
    bool loaded = false;
    bool unlocked;
    bool global;
    LuaScriptCallback c_command;

    LuaScriptCallback c_frame;
    LuaScriptCallback c_preframe;

    LuaScriptCallback c_map_load;
    LuaScriptCallback c_map_preload;

    LuaScriptCallback c_rcon_message;

    LuaScriptCallback c_spawn;
    LuaScriptCallback c_prespawn;

    LuaScriptCallback c_tick;
    LuaScriptCallback c_pretick;

    LuaScriptCallback c_unload;

    size_t next_timer_id = 0;

    LuaScript(lua_State *state, const char *name, const bool &global, const bool &unlocked) noexcept;
    ~LuaScript() noexcept;
};

LuaScript &script_from_state(lua_State *state) noexcept;
void refresh_client_index(lua_State *state) noexcept;
void refresh_variables(lua_State *state) noexcept;
void print_error(lua_State *state) noexcept;
