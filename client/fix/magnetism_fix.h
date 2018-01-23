#pragma once

/// Return true if a gamepad is present.
bool gamepad_plugged_in() noexcept;

/// Fix Halo's magnetism problems if a gamepad is present.
void fix_magnetism() noexcept;
