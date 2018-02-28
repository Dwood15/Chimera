#include "server_ip.h"

#include "../messaging/messaging.h"
#include "../client_signature.h"

ChimeraCommandError block_server_ip_command(size_t argc, const char **argv) noexcept {
    static auto active = false;
    if(argc == 1) {
        bool new_value = bool_value(argv[0]);
        if(new_value != active) {
            auto &join_server_ip_text_sig = get_signature("join_server_ip_text_sig");
            auto &f1_ip_text_render_call_sig = get_signature("f1_ip_text_render_call_sig");
            auto &create_server_ip_text_sig = get_signature("create_server_ip_text_sig");
            if(new_value) {
                const unsigned char mod[] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
                write_code_c(join_server_ip_text_sig.address() + 5, mod);
                write_code_c(f1_ip_text_render_call_sig.address(), mod);

                const short mod_create_server_ip[] = { 0xB9, 0x00, 0x00, 0x00, 0x00, 0x90, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0x66, 0xB9, 0x00, 0x00, 0x90, 0x90, 0x90 };
                write_code_s(create_server_ip_text_sig.address(), mod_create_server_ip);
            }
            else {
                join_server_ip_text_sig.undo();
                f1_ip_text_render_call_sig.undo();
                create_server_ip_text_sig.undo();
            }
            active = new_value;
        }
    }
    console_out(active ? "true" : "false");
    return CHIMERA_COMMAND_ERROR_SUCCESS;
}
