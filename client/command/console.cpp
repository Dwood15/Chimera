#include "console.h"
#include "../messaging/messaging.h"
#include "../client_signature.h"

static char *console_text = NULL;

static void block_error() {
    auto *push_req = get_signature("console_block_error_sig").address();
    DWORD old_protect = 0;
    DWORD old_protect_b = 0;
    if(VirtualProtect(push_req, 5, PAGE_READWRITE, &old_protect) == false) return;
    for(int i=0;i<5;i++) *(push_req + 6 + i) = 0x90;
    VirtualProtect(push_req, 5, old_protect, &old_protect_b);
}

static void unblock_error() {
    get_signature("console_block_error_sig").undo();
}

static void read_command() {
    block_error();
    if(strlen(console_text) > 127) {
        unblock_error();
        return;
    }
    switch(execute_chimera_command(console_text)) {
        case CHIMERA_COMMAND_ERROR_NOT_ENOUGH_ARGUMENTS: {
            auto &f = find_chimera_command(console_text);
            char text[256] = {};
            sprintf(text,"Error: Function %s takes at least %u argument%s.", f.name(), f.min_args(), f.min_args() == 1 ? "" : "s");
            console_out_error(text);
            break;
        }
        case CHIMERA_COMMAND_ERROR_TOO_MANY_ARGUMENTS: {
            auto &f = find_chimera_command(console_text);
            char text[256] = {};
            sprintf(text,"Error: Function %s takes no more than %u argument%s.", f.name(), f.max_args(), f.max_args() == 1 ? "" : "s");
            console_out_error(text);
            break;
        }
        case CHIMERA_COMMAND_ERROR_UNSUPPORTED: {
            auto &f = find_chimera_command(console_text);
            char text[256] = {};
            sprintf(text,"Error: Function %s is unsupported on your Halo client.", f.name());
            console_out_error(text);
            break;
        }
        case CHIMERA_COMMAND_ERROR_COMMAND_NOT_FOUND: {
            extern bool on_command_lua(const char *command);
            if(on_command_lua(console_text)) {
                unblock_error();
            }
            else {
                console_text[0] = 0;
            }
            break;
        }
        default: {}
    }
}

void initialize_console() {
    auto &console_call_s = get_signature("console_call_sig");
    auto *console_ptr = console_call_s.address();
    console_text = I8PTR(*reinterpret_cast<uint32_t *>(console_ptr - 4));
    static BasicCodecave console_codecave;
    write_jmp_call(console_call_s.address(), reinterpret_cast<void *>(read_command), nullptr, console_codecave);
}

bool console_is_out() {
    static auto *out = *reinterpret_cast<char **>(get_signature("console_is_out_sig").address() + 2);
    return *out;
}

bool already_set = false;

ChimeraCommandError enable_console_command(size_t argc, const char **argv) noexcept {
    extern bool initial_tick;
    static bool active = true;
    if(argc == 1) {
        auto new_value = bool_value(argv[0]);
        if(!already_set && new_value != active) {
            if(!initial_tick) {
                console_out("chimera_enable_console: Changes will take effect after you relaunch Halo.");
                already_set = true;
            }
            else {
                **reinterpret_cast<char **>(get_signature("enable_console_sig").address() + 1) = new_value;
            }
        }
        active = new_value;
    }
    console_out(std::string("chimera_enable_console: ") + (active ? "true" : "false"));
    return CHIMERA_COMMAND_ERROR_SUCCESS;
}
