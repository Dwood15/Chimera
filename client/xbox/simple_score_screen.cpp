#include "simple_score_screen.h"

#include "../client_signature.h"
#include "../messaging/messaging.h"

ChimeraCommandError simple_score_screen_command(size_t argc, const char **argv) noexcept {
    static auto active = false;
    if(argc == 1) {
        bool new_value = bool_value(argv[0]);
        if(new_value != active) {
            auto &ss_elements_sig = get_signature("ss_elements_sig");
            auto &ss_score_header_sig = get_signature("ss_score_header_sig");
            auto &ss_score_background_sig = get_signature("ss_score_background_sig");
            if(new_value) {
                auto *ss_elements_addr = ss_elements_sig.address();
                write_code_any_value(ss_elements_addr + 7*0 + 5, static_cast<unsigned short>(0x7FFF)); // placement
                write_code_any_value(ss_elements_addr + 7*1 + 5, static_cast<unsigned short>(0x00CC)); // name
                write_code_any_value(ss_elements_addr + 7*2 + 5, static_cast<unsigned short>(0x0168)); // score
                write_code_any_value(ss_elements_addr + 7*3 + 5, static_cast<unsigned short>(0x7FFF)); // kills
                write_code_any_value(ss_elements_addr + 7*4 + 5, static_cast<unsigned short>(0x7FFF)); // assists
                write_code_any_value(ss_elements_addr + 7*5 + 5, static_cast<unsigned short>(0x7FFF)); // deaths
                write_code_any_value(ss_elements_addr + 7*6 + 2, static_cast<unsigned short>(0x01A4)); // ping

                unsigned char nope[] = {0x90, 0x90, 0x90, 0x90, 0x90};
                write_code_c(ss_score_header_sig.address(), nope);
                write_code_c(ss_score_background_sig.address() + 52, nope);
            }
            else {
                ss_elements_sig.undo();
                ss_score_header_sig.undo();
                ss_score_background_sig.undo();
            }
            active = new_value;
        }
    }
    console_out(std::string("chimera_simple_score_screen: ") + (active ? "true" : "false"));
    return CHIMERA_COMMAND_ERROR_SUCCESS;
}
