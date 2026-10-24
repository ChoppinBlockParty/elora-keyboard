// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

// Shared with tests/qmk_tap_hold so the tests cannot drift from the firmware.

#define TAPPING_TERM 200
// Zero disables tap repeat, so a tap followed by a hold still holds.
#define QUICK_TAP_TERM 0
// Resolve a dual key by which hands are involved.
#define CHORDAL_HOLD
#define HOLD_ON_OTHER_KEY_PRESS_PER_KEY
#define PERMISSIVE_HOLD_PER_KEY
