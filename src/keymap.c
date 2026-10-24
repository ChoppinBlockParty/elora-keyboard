// SPDX-License-Identifier: GPL-2.0-or-later
#include QMK_KEYBOARD_H
#include "tap_hold.h"

#define TRNS KC_TRNS
#define XXX  KC_NO

// Ordering follows SplitKB's Elora rev2 default_hlc LAYOUT, including its
// four row-four extras and ten thumb positions.
// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [BASE] = LAYOUT(
        KC_ESC,    KC_1,      KC_2,      KC_3,      KC_4,      KC_5,                                                  KC_6,      KC_7,      KC_8,      KC_9,      KC_0,      KC_ESC,
        KC_TAB,    KC_Q,      KC_W,      KC_E,      KC_R,      KC_T,                                                  KC_Y,      KC_U,      KC_I,      KC_O,      KC_P,      KC_BSPC,
        ESC_CTRL,  A_GUI,     S_ALT,     D_CTRL,    F_SHIFT,   KC_G,                                                  KC_H,      J_SHIFT,   K_CTRL,    L_ALT,     SCLN_GUI,  QUOT_CTRL,
        KC_LSFT,   KC_Z,      KC_X,      KC_C,      KC_V,      KC_B,      XXX,       KC_UP,     KC_RGHT,   XXX,       KC_N,      KC_M,      KC_COMM,   KC_DOT,    KC_SLSH,   KC_RSFT,
                                         XXX,       ESC_MEDIA, SPC_NAV,   TAB_MOUSE, KC_DOWN,   KC_LEFT,   ENT_SYM,   BSPC_NUM,  DEL_FUN,   XXX
    ),
    [NAV] = LAYOUT(
        TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,                                                  TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,
        TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,                                                  KC_HOME,   KC_PGDN,   KC_PGUP,   KC_END,    TRNS,      TRNS,
        TRNS,      KC_LGUI,   KC_LALT,   KC_LCTL,   KC_LSFT,   TRNS,                                                  KC_LEFT,   KC_DOWN,   KC_UP,     KC_RGHT,   TRNS,      TRNS,
        TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,
                                         TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS
    ),
    [SYM] = LAYOUT(
        TRNS,      KC_GRV,    KC_TILD,   KC_BSLS,   KC_MINS,   KC_EQL,                                                TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,
        TRNS,      KC_EXLM,   KC_AT,     KC_HASH,   KC_DLR,    KC_PERC,                                               TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,
        TRNS,      KC_CIRC,   KC_AMPR,   KC_ASTR,   KC_LPRN,   KC_RPRN,                                               TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,
        TRNS,      KC_LBRC,   KC_RBRC,   KC_LCBR,   KC_RCBR,   KC_PIPE,   TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,
                                         TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS
    ),
    [NUM] = LAYOUT(
        TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,                                                  TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,
        TRNS,      TRNS,      KC_7,      KC_8,      KC_9,      TRNS,                                                  TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,
        TRNS,      TRNS,      KC_4,      KC_5,      KC_6,      KC_0,                                                  TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,
        TRNS,      TRNS,      KC_1,      KC_2,      KC_3,      KC_DOT,    TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,
                                         TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS
    ),
    [FUN] = LAYOUT(
        TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,                                                  TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,
        TRNS,      KC_F1,     KC_F2,     KC_F3,     KC_F4,     KC_F5,                                                 TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,
        TRNS,      KC_F6,     KC_F7,     KC_F8,     KC_F9,     KC_F10,                                                TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,
        TRNS,      KC_F11,    KC_F12,    TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,
                                         TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS
    ),
    [MEDIA] = LAYOUT(
        TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,                                                  TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,
        TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,                                                  TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,
        TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,                                                  TRNS,      KC_MPRV,   KC_VOLD,   KC_VOLU,   KC_MNXT,   TRNS,
        TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      KC_MPLY,   KC_MUTE,   TRNS,      TRNS,      TRNS,      TRNS,
                                         TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS
    ),
    [MOUSE] = LAYOUT(
        TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,                                                  TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,
        TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,                                                  TRNS,      TRNS,      MS_UP,     TRNS,      TRNS,      TRNS,
        TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,                                                  TRNS,      MS_LEFT,   MS_DOWN,   MS_RGHT,   TRNS,      TRNS,
        TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      MS_BTN1,   MS_BTN2,   MS_BTN3,   TRNS,      TRNS,      TRNS,
                                         TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS,      TRNS
    ),
};
// clang-format on

bool encoder_update_user(uint8_t index, bool clockwise) {
    (void)index;
    tap_code16(clockwise ? KC_TAB : S(KC_TAB));
    return false;
}

#ifdef HALCYON_ENABLE
// Unassigned until a behavior is chosen for the encoder's push switch.
const uint16_t left_halcyon_buttons[10][5] = {{KC_NO}};
const uint16_t right_halcyon_buttons[10][5] = {{KC_NO}};
#endif
