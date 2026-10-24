// SPDX-License-Identifier: GPL-2.0-or-later
#include "tap_hold.h"
#include <string.h>
#ifdef ELORA_KEYLOG
#    include "keylog.h"
#endif

static bool is_modifier_tap(uint16_t keycode) {
    switch (keycode) {
        case A_GUI:
        case S_ALT:
        case D_CTRL:
        case F_SHIFT:
        case J_SHIFT:
        case K_CTRL:
        case L_ALT:
        case SCLN_GUI:
        case ESC_CTRL:
        case QUOT_CTRL:
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
        case ESC_MEDIA:
        case SPC_NAV:
        case TAB_MOUSE:
        case ENT_SYM:
        case BSPC_NUM:
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

// QMK stores one bit per modifier, so when two keys supply the same bit,
// letting it process the first release clears the bit while the other key is
// still down. Each group owns a bit and withholds the release until every
// source for it is up.
//
// Only Shift and Ctrl need a group. No two keys here supply the same Alt or
// GUI bit: Nav's Alt and GUI sit on the same physical keys as their Base
// mod-taps, and L_ALT and SCLN_GUI are the right-hand variants. Giving any
// modifier a second key means giving it a group.
#define SHARED_MODIFIER_HOLD_SOURCES 2
#define SHARED_MODIFIER_NO_SOURCE    UINT8_MAX

typedef struct {
    uint8_t mod_bit;
    bool hold_source[SHARED_MODIFIER_HOLD_SOURCES];
    uint8_t basic_press_count;
} shared_modifier_t;

static shared_modifier_t left_shift = {.mod_bit = MOD_BIT(KC_LEFT_SHIFT)};
static shared_modifier_t right_shift = {.mod_bit = MOD_BIT(KC_RIGHT_SHIFT)};
static shared_modifier_t left_ctrl = {.mod_bit = MOD_BIT(KC_LEFT_CTRL)};
static shared_modifier_t right_ctrl = {.mod_bit = MOD_BIT(KC_RIGHT_CTRL)};

// Pass SHARED_MODIFIER_NO_SOURCE as `except` to test every source.
static bool other_sources_held(const shared_modifier_t *group, uint8_t except) {
    if (group->basic_press_count > 0) {
        return true;
    }
    for (uint8_t source = 0; source < SHARED_MODIFIER_HOLD_SOURCES; ++source) {
        if (source != except && group->hold_source[source]) {
            return true;
        }
    }
    return false;
}

// USB suspend and a host driver change clear QMK's modifier bits without
// sending any release. A group that kept believing its key was down would then
// withhold the next real release and stick the modifier on.
static void forget_stale_sources(shared_modifier_t *group) {
    if (get_mods() & group->mod_bit) {
        return;
    }
    memset(group->hold_source, 0, sizeof(group->hold_source));
    group->basic_press_count = 0;
}

static bool process_hold_source(shared_modifier_t *group, uint8_t source, keyrecord_t *record) {
    forget_stale_sources(group);
    if (record->tap.count != 0) {
        return true; // Resolved as a tap, so it registers no modifier.
    }
    group->hold_source[source] = record->event.pressed;
    return record->event.pressed || !other_sources_held(group, source);
}

static bool process_basic_source(shared_modifier_t *group, keyrecord_t *record) {
    forget_stale_sources(group);
    if (record->event.pressed) {
        ++group->basic_press_count;
        return true;
    }
    if (group->basic_press_count > 0) {
        --group->basic_press_count;
    }
    return group->basic_press_count == 0 && !other_sources_held(group, SHARED_MODIFIER_NO_SOURCE);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
#ifdef ELORA_KEYLOG
    keylog_note_keypress(keycode, record);
#endif
    switch (keycode) {
        case F_SHIFT: return process_hold_source(&left_shift, 0, record);
        case KC_LSFT: return process_basic_source(&left_shift, record);
        case J_SHIFT: return process_hold_source(&right_shift, 0, record);
        case KC_RSFT: return process_basic_source(&right_shift, record);
        case D_CTRL: return process_hold_source(&left_ctrl, 0, record);
        case ESC_CTRL: return process_hold_source(&left_ctrl, 1, record);
        case KC_LCTL: return process_basic_source(&left_ctrl, record);
        case K_CTRL: return process_hold_source(&right_ctrl, 0, record);
        case QUOT_CTRL: return process_hold_source(&right_ctrl, 1, record);
        case KC_RCTL: return process_basic_source(&right_ctrl, record);
        default: break;
    }
    return true;
}
