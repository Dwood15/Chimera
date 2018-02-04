#include "split_screen_hud.h"

#include "../hac2/hac2.h"
#include "../client_signature.h"
#include "../messaging/messaging.h"

ChimeraCommandError split_screen_hud_command(size_t argc, const char **argv) noexcept {
    static auto active = false;
    if(argc == 1) {
        bool new_value = bool_value(argv[0]);
        if(new_value != active) {
            auto &ammo_counter_ss_sig = get_signature("ammo_counter_ss_sig");
            auto &hud_text_ss_sig = get_signature("hud_text_ss_sig");
            auto &split_screen_hud_ss_sig = get_signature("split_screen_hud_ss_sig");
            auto &hac2_workaround_ss_sig = get_signature("hac2_workaround_ss_sig");
            if(new_value) {
                const short ammo_counter_mod[] = {-1,   0xB8, 0x02, 0x00};
                const short hud_text_mod[] = {-1,   0xB8, 0x02, 0x00};
                const short split_screen_hud_mod[] = {-1,   -1,   -1,   -1,   0x00};
                write_code_s(ammo_counter_ss_sig.address(), ammo_counter_mod);
                write_code_s(hud_text_ss_sig.address(), hud_text_mod);
                write_code_s(split_screen_hud_ss_sig.address(), split_screen_hud_mod);

                // workaround to keep Halo from crashing if HAC2 is present when split screen is enabled
                if(hac2_present()) {
                    static unsigned char bytecode[] = {
                        // cmp eax, 0x00
                        0x83, 0xF8, 0x00,

                        // jne :continue_fn
                        0x75, 0x01,

                        // ret
                        0xC3,

                        // :continue_fn
                        // sub esp, 0x08
                        0x83, 0xEC, 0x08,

                        // push ebx
                        0x53,

                        // push ebp
                        0x55,

                        // jmp function()+5
                        0xE9, 0xFF, 0xFF, 0xFF, 0xFF
                    };

                    auto *hac2_workaround_ss_address = hac2_workaround_ss_sig.address();
                    *reinterpret_cast<uint32_t *>(bytecode + 11 + 1) = reinterpret_cast<uint32_t>(hac2_workaround_ss_address + 5) - reinterpret_cast<uint32_t>(bytecode + 11 + 5);
                    write_code_any_value(hac2_workaround_ss_address + 0, static_cast<unsigned char>(0xE9));
                    write_code_any_value(hac2_workaround_ss_address + 1, reinterpret_cast<uint32_t>(bytecode) - reinterpret_cast<uint32_t>(hac2_workaround_ss_address + 5));
                }
            }
            else {
                ammo_counter_ss_sig.undo();
                hud_text_ss_sig.undo();
                split_screen_hud_ss_sig.undo();

                if(hac2_present()) {
                    hac2_workaround_ss_sig.undo();
                }
            }
            active = new_value;
        }
    }
    console_out(std::string("chimera_split_screen_hud: ") + (active ? "true" : "false"));
    return CHIMERA_COMMAND_ERROR_SUCCESS;
}
