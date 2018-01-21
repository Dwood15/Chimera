#include "../client_signature.h"
#include "messaging.h"

bool silence_all_messages = false;

void console_out(const char *text, const ColorARGB &color) noexcept {
    if(silence_all_messages) return;
    static auto *console_out_address = get_signature("console_out_sig").address();
    asm (
        "pushad;"
        "mov eax, %0;"
        "push %1;"
        "push %2;"
        "call %3;"
        "add esp, 8;"
        "popad;"
        :
        : "r" (&color), "r" (text), "r" ("%s"), "r" (console_out_address)
    );
}

void console_out(const std::string &text, const ColorARGB &color) noexcept {
    return console_out(text.data(), color);
}

void console_out_warning(const char *text) noexcept {
    ColorARGB color;
    color.alpha = 1.0;
    color.red = 1.0;
    color.green = 1.0;
    color.blue = 0.2;
    console_out(text, color);
}

void console_out_warning(const std::string &text) noexcept {
    return console_out_warning(text.data());
}

void console_out_error(const char *text) noexcept {
    ColorARGB color;
    color.alpha = 1.0;
    color.red = 1.0;
    color.green = 0.2;
    color.blue = 0.2;
    bool already_silent = silence_all_messages;
    silence_all_messages = false;
    console_out(text, color);
    silence_all_messages = already_silent;
}

void console_out_error(const std::string &text) noexcept {
    return console_out_error(text.data());
}

void hud_message(const char *message) noexcept {
    if(silence_all_messages) return;
    static auto *hud_message_address = get_signature("hud_message_sig").address();

    size_t mlen = strlen(message);
    short copy[64] = {};
    if(mlen + 1 > sizeof(copy)/sizeof(copy[0])) return;
    for(int i=0;i<mlen;i++) {
        copy[i] = (short)message[i];
    }

    asm (
        "pushad;"
        "push %0;"
        "mov eax, 0;"
        "call %1;"
        "add esp, 4;"
        "popad;"
        :
        : "r" (copy), "r" (hud_message_address)
    );
}

void hud_message(const std::string &message) noexcept {
    return hud_message(message.data());
}
