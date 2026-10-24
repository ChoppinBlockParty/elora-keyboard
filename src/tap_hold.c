// SPDX-License-Identifier: GPL-2.0-or-later
#include "tap_hold.h"
#ifdef ELORA_KEYLOG
#    include "keylog.h"
#endif

static bool is_modifier_tap(uint16_t keycode) {
    switch (keycode) {
        case GUI_A:
        case ALT_S:
        case CTL_D:
        case SFT_F:
        case SFT_J:
        case CTL_K:
        case ALT_L:
        case GUI_SCL:
        case CTL_ESC:
        case CTL_QUO:
            return true;
        default:
            return false;
    }
}

// Resolve opposite-hand layer chords immediately. Modifier-tap keys use
// permissive hold instead, so nested shortcuts work without turning a
// same-hand letter roll into a modifier chord.
bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) {
    (void)record;
    switch (keycode) {
        case ESC_MED:
        case SPC_NAV:
        case TAB_MOU:
        case ENT_SYM:
        case BSP_NUM:
        case DEL_FUN:
            return true;
        default:
            return false;
    }
}

bool get_permissive_hold(uint16_t keycode, keyrecord_t *record) {
    (void)record;
    return is_modifier_tap(keycode);
}

bool get_chordal_hold(uint16_t tap_hold_keycode, keyrecord_t *tap_hold_record,
                      uint16_t other_keycode, keyrecord_t *other_record) {
    (void)other_keycode;
    // Resolve every modifier-tap by event order, regardless of which hand
    // presses the second key. Permissive Hold makes a nested tap a modifier;
    // releasing the modifier key first keeps a rolling pair as two taps.
    if (is_modifier_tap(tap_hold_keycode)) {
        return true;
    }
    return get_chordal_hold_default(tap_hold_record, other_record);
}

// QMK stores held modifiers as bits. Releasing one source clears that bit
// even if another source on the same side is still down. Keep it set until
// all overlapping Shift or Ctrl sources have been released.
static bool left_home_shift_held;
static bool right_home_shift_held;
static bool left_home_ctrl_held;
static bool left_outer_ctrl_held;
static bool right_home_ctrl_held;
static bool right_outer_ctrl_held;
static uint8_t left_basic_shift_count;
static uint8_t right_basic_shift_count;
static uint8_t left_basic_ctrl_count;
static uint8_t right_basic_ctrl_count;

static bool process_shared_modifier(bool *this_held, bool other_held, keyrecord_t *record, bool is_mod_tap) {
    if (is_mod_tap && record->tap.count != 0) {
        return true;
    }
    *this_held = record->event.pressed;
    return record->event.pressed || !other_held;
}

static bool process_basic_modifier(uint8_t *held_count, bool other_held, keyrecord_t *record) {
    if (record->event.pressed) {
        ++*held_count;
        return true;
    }
    if (*held_count > 0) {
        --*held_count;
    }
    return *held_count == 0 && !other_held;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
#ifdef ELORA_KEYLOG
    keylog_note_keypress(keycode, record);
#endif
    switch (keycode) {
        case SFT_F:
            return process_shared_modifier(&left_home_shift_held, left_basic_shift_count > 0, record, true);
        case SFT_J:
            return process_shared_modifier(&right_home_shift_held, right_basic_shift_count > 0, record, true);
        case KC_LSFT:
            return process_basic_modifier(&left_basic_shift_count, left_home_shift_held, record);
        case KC_RSFT:
            return process_basic_modifier(&right_basic_shift_count, right_home_shift_held, record);
        case CTL_D:
            return process_shared_modifier(&left_home_ctrl_held, left_outer_ctrl_held || left_basic_ctrl_count > 0, record, true);
        case CTL_ESC:
            return process_shared_modifier(&left_outer_ctrl_held, left_home_ctrl_held || left_basic_ctrl_count > 0, record, true);
        case CTL_K:
            return process_shared_modifier(&right_home_ctrl_held, right_outer_ctrl_held || right_basic_ctrl_count > 0, record, true);
        case CTL_QUO:
            return process_shared_modifier(&right_outer_ctrl_held, right_home_ctrl_held || right_basic_ctrl_count > 0, record, true);
        case KC_LCTL:
            return process_basic_modifier(&left_basic_ctrl_count, left_home_ctrl_held || left_outer_ctrl_held, record);
        case KC_RCTL:
            return process_basic_modifier(&right_basic_ctrl_count, right_home_ctrl_held || right_outer_ctrl_held, record);
    }
    return true;
}
