#include "controls.h"
#include "windows.h"

bool controls_present() noexcept {
    static bool controls_present = GetModuleHandle("controls.dll") != nullptr;
    return controls_present;
}
