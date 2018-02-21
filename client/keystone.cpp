#include "keystone.h"

#include "client_signature.h"
#include "messaging/messaging.h"
#include "settings.h"

static void handle_keystone_text(const short *text) {
    char b[128] = {};
    for(int i=0;i<sizeof(b)-1;i++) {
        switch(text[i]) {
            case 0: break;
            default: b[i] = text[i];
        }
    }
    console_out(b);
}

void setup_keystone_override() noexcept {
    auto &on_keystone_message_sig = get_signature("on_keystone_message_sig");
    write_code_any_value(on_keystone_message_sig.address(), static_cast<unsigned char>(0xE9));
    write_code_any_value(on_keystone_message_sig.address() + 1, reinterpret_cast<int>(handle_keystone_text) - reinterpret_cast<int>(on_keystone_message_sig.address() + 5));
}

bool keystone_enabled = false;

ChimeraCommandError keystone_command(size_t argc, const char **argv) noexcept {
    static bool active = false;
    extern bool first_tick;
    if(argc == 1) {
        bool new_value = bool_value(argv[0]);
        if(new_value != active) {
            if(!first_tick) {
                setup_keystone_override();
                keystone_enabled = true;
            }
            startup_parameters().fast_startup = active;
            save_all_changes();
            active = new_value;
        }
    }
    console_out(active ? "true" : "false");
    return CHIMERA_COMMAND_ERROR_SUCCESS;
}
