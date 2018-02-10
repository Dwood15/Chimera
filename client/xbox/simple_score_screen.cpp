#include "simple_score_screen.h"

#include "../client_signature.h"
#include "../messaging/messaging.h"

ChimeraCommandError simple_score_screen_command(size_t argc, const char **argv) noexcept {
    static auto active = false;
    if(argc == 1) {
        bool new_value = bool_value(argv[0]);
        if(new_value != active) {
            auto &ss_elements_sig = get_signature("ss_elements_sig");
            auto &ss_score_background_sig = get_signature("ss_score_background_sig");
            auto &ss_score_position_sig = get_signature("ss_score_position_sig");
            if(new_value) {

                #define CENTER_POS 300
                #define NAME_POS CENTER_POS - 70
                #define PLACE_POS NAME_POS - 50
                #define SCORE_POS CENTER_POS + 60
                #define PING_POS SCORE_POS + 60
                #define HEADER_POS PLACE_POS - 20

                static short header[2] = { HEADER_POS, HEADER_POS };
                auto *ss_elements_addr = ss_elements_sig.address();
                write_code_any_value(ss_elements_addr + 2, header);
                write_code_any_value(ss_elements_addr + 0x2E + 7*0 + 5, static_cast<short>(PLACE_POS)); // placement
                write_code_any_value(ss_elements_addr + 0x2E + 7*1 + 5, static_cast<short>(NAME_POS)); // name
                write_code_any_value(ss_elements_addr + 0x2E + 7*2 + 5, static_cast<short>(SCORE_POS)); // score
                write_code_any_value(ss_elements_addr + 0x2E + 7*3 + 5, static_cast<short>(0x7FFF)); // kills
                write_code_any_value(ss_elements_addr + 0x2E + 7*4 + 5, static_cast<short>(0x7FFF)); // assists
                write_code_any_value(ss_elements_addr + 0x2E + 7*5 + 5, static_cast<short>(0x7FFF)); // deaths
                write_code_any_value(ss_elements_addr + 0x2E + 7*6 + 2, static_cast<short>(PING_POS)); // ping

                auto *ss_score_position_addr = ss_score_position_sig.address();
                write_code_any_value(ss_score_position_addr + 2, static_cast<char>(0x10));
                write_code_any_value(ss_score_position_addr + 7 + 2, static_cast<char>(0x60));

                unsigned char nope[] = {0x90, 0x90, 0x90, 0x90, 0x90};
                write_code_c(ss_score_background_sig.address() + 52, nope);
            }
            else {
                ss_elements_sig.undo();
                ss_score_background_sig.undo();
                ss_score_position_sig.undo();
            }
            active = new_value;
        }
    }
    console_out(active ? "true" : "false");
    return CHIMERA_COMMAND_ERROR_SUCCESS;
}
