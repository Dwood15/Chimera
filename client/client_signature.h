#pragma once

struct BasicCodecave {
    unsigned char data[64];
};
void write_jmp_call(void *call_instruction, void *before_function, void *after_function, BasicCodecave &codecave) noexcept;

#include "../code_injection/signature.h"
ChimeraSignature &get_signature(const char *name) noexcept;

bool find_required_signatures() noexcept;
bool find_interpolation_signatures() noexcept;
bool find_uncap_cinematic_signatures() noexcept;
bool find_magnetism_signatures() noexcept;
bool find_auto_center_signature() noexcept;
bool find_widescreen_scope_signature() noexcept;
bool find_zoom_blur_signatures() noexcept;
bool find_anisotropic_filtering_signature() noexcept;
bool find_debug_signatures() noexcept;
bool find_loading_screen_signatures() noexcept;
bool find_multitexture_overlay_signature() noexcept;
bool find_set_resolution_signatures() noexcept;
bool find_widescreen_fix_signatures() noexcept;
bool find_gametype_indicator_sig() noexcept;
bool find_devmode_sig() noexcept;
bool find_simple_score_screen_sigs() noexcept;
bool find_split_screen_hud_sigs() noexcept;

#define I32(x) reinterpret_cast<int32_t>(x)
#define I32PTR(x) reinterpret_cast<int32_t *>(x)

#define I8(x) reinterpret_cast<char>(x)
#define I8PTR(x) reinterpret_cast<char *>(x)
