#include "script.h"

#include "../client_signature.h"

void execute_script(const char *script) noexcept {
    static auto *execute_script_address = get_signature("execute_script_sig").address();
    asm (
        "pushad;"
        "push %0;"
        "call %1;"
        "add esp, 4;"
        "popad;"
        :
        : "r" (script), "r" (execute_script_address)
    );
}
