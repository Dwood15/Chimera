#include "chat.h"

#include "../client_signature.h"
#include "../command/console.h"
#include "../messaging/messaging.h"

void chat_out(const char *message, uint32_t channel) noexcept {
    typedef void *(*send_chat_fn)(uint32_t, const short *, const short *);
    extern ChimeraSignature chat_out_s;
    static send_chat_fn send_chat = (send_chat_fn)get_signature("chat_out_sig").address();
    #define BLEN 256
    short *bullshit = new short[BLEN+1]();
    short *bullshit2 = new short[BLEN+1]();
    size_t x = strlen(message);
    if(x > BLEN) {
        return;
    }

    for(int i=0;i<x;i++) {
        bullshit[i] = (short)message[i];
        bullshit2[i] = (short)message[i];
    }

    send_chat(channel, bullshit, bullshit2);
    delete[] bullshit;
    delete[] bullshit2;
}

ChimeraCommandError chat_command(size_t argc, const char **argv) noexcept {
    if(argc == 1) {
        console_is_out(false);
        return CHIMERA_COMMAND_ERROR_SUCCESS;
    }
    auto method = std::string(argv[0]);
    int channel;
    if(method == "all") channel = 0;
    else if(method == "team") channel = 1;
    else if(method == "vehicle") channel = 2;
    else {
        console_out_error("Expected all, team, or vehicle for argument #1.");
        return CHIMERA_COMMAND_ERROR_FAILURE;
    }
    auto message = std::string(argv[1]);
    for(size_t i=2;i<argc;i++) {
        message = message + " " + argv[2];
    }
    chat_out(message.data(), channel);
    console_is_out(false);
    return CHIMERA_COMMAND_ERROR_SUCCESS;
}
