#pragma once

#include <string>

struct ColorARGB {
    float alpha = 1.0;
    float red = 1.0;
    float green = 1.0;
    float blue = 1.0;

    ColorARGB() {}
    ColorARGB(float a, float r, float g, float b) : alpha(a), red(r), green(g), blue(b) {}
    ColorARGB(const ColorARGB &other) : alpha(other.alpha), red(other.red), green(other.green), blue(other.blue) {}
};

/// Output a C string to the console while optionally using a color.
void console_out(const char *text, const ColorARGB &color = ColorARGB()) noexcept;

/// Output a C++ string to the console with a C++ string while optionally using a color.
void console_out(const std::string &text, const ColorARGB &color = ColorARGB()) noexcept;

/// Output a C string to the console using yellow text. This is used for warnings.
void console_out_warning(const char *text) noexcept;

/// Output a C++ string to the console using yellow text. This is used for warnings.
void console_out_warning(const std::string &text) noexcept;

/// Output a C string to the console using red text. This is used for errors.
void console_out_error(const char *text) noexcept;

/// Output a C++ string to the console using red text. This is used for errors.
void console_out_error(const std::string &text) noexcept;

/// Output a C string to the HUD.
void hud_message(const char *message) noexcept;

/// Output a C++ string to the HUD.
void hud_message(const char *message) noexcept;
