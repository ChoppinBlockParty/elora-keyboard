// SPDX-License-Identifier: GPL-2.0-or-later
#include QMK_KEYBOARD_H

enum layers { BASE, NAV, SYM, NUM, FUN, MEDIA, MOUSE };

#define GUI_A   LGUI_T(KC_A)
#define ALT_S   LALT_T(KC_S)
#define CTL_D   LCTL_T(KC_D)
#define SFT_F   LSFT_T(KC_F)
#define SFT_J   RSFT_T(KC_J)
#define CTL_K   RCTL_T(KC_K)
#define ALT_L   RALT_T(KC_L)
#define GUI_SCL RGUI_T(KC_SCLN)
#define CTL_ESC LCTL_T(KC_ESC)
#define CTL_QUO RCTL_T(KC_QUOT)
#define ESC_MED LT(MEDIA, KC_ESC)
#define SPC_NAV LT(NAV, KC_SPC)
#define TAB_MOU LT(MOUSE, KC_TAB)
#define ENT_SYM LT(SYM, KC_ENT)
#define BSP_NUM LT(NUM, KC_BSPC)
#define DEL_FUN LT(FUN, KC_DEL)

#define ___ KC_TRNS
#define XXX KC_NO

// Ordering follows SplitKB's Elora rev2 default_hlc LAYOUT, including its
// four row-four extras and ten thumb positions.
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [BASE] = LAYOUT(
        KC_ESC,  KC_1, KC_2, KC_3, KC_4, KC_5,                 KC_6, KC_7, KC_8, KC_9, KC_0, KC_ESC,
        KC_TAB,  KC_Q, KC_W, KC_E, KC_R, KC_T,                 KC_Y, KC_U, KC_I, KC_O, KC_P, KC_BSPC,
        CTL_ESC, GUI_A, ALT_S, CTL_D, SFT_F, KC_G,             KC_H, SFT_J, CTL_K, ALT_L, GUI_SCL, CTL_QUO,
        KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, XXX, KC_UP,     KC_RGHT, XXX, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_RSFT,
                            XXX, ESC_MED, SPC_NAV, TAB_MOU, KC_DOWN,  KC_LEFT, ENT_SYM, BSP_NUM, DEL_FUN, XXX
    ),
    [NAV] = LAYOUT(
        ___, ___, ___, ___, ___, ___,                            ___, ___, ___, ___, ___, ___,
        ___, ___, ___, ___, ___, ___,                            KC_HOME, KC_PGDN, KC_PGUP, KC_END, ___, ___,
        ___, KC_LGUI, KC_LALT, KC_LCTL, KC_LSFT, ___,            KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, ___, ___,
        ___, ___, ___, ___, ___, ___, ___, ___,                  ___, ___, ___, ___, ___, ___, ___, ___,
                            ___, ___, ___, ___, ___,             ___, ___, ___, ___, ___
    ),
    [SYM] = LAYOUT(
        ___, ___, ___, ___, ___, ___,                            ___, ___, ___, ___, ___, ___,
        ___, KC_EXLM, KC_AT, KC_HASH, KC_DLR, KC_PERC,          KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, ___,
        ___, KC_GRV, KC_TILD, KC_BSLS, KC_PIPE, KC_LBRC,        KC_RBRC, KC_LCBR, KC_RCBR, KC_MINS, KC_EQL, ___,
        ___, ___, ___, ___, ___, ___, ___, ___,                  ___, ___, ___, ___, ___, ___, ___, ___,
                            ___, ___, ___, ___, ___,             ___, ___, ___, ___, ___
    ),
    [NUM] = LAYOUT(
        ___, ___, ___, ___, ___, ___,                            ___, ___, ___, ___, ___, ___,
        ___, ___, ___, ___, ___, ___,                            KC_7, KC_8, KC_9, ___, ___, ___,
        ___, ___, ___, ___, ___, ___,                            KC_4, KC_5, KC_6, KC_0, ___, ___,
        ___, ___, ___, ___, ___, ___, ___, ___,                  ___, ___, KC_1, KC_2, KC_3, KC_DOT, ___, ___,
                            ___, ___, ___, ___, ___,             ___, ___, ___, ___, ___
    ),
    [FUN] = LAYOUT(
        ___, ___, ___, ___, ___, ___,                            ___, ___, ___, ___, ___, ___,
        ___, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5,                  KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, ___,
        ___, KC_F11, KC_F12, ___, ___, ___,                     ___, ___, ___, ___, ___, ___,
        ___, ___, ___, ___, ___, ___, ___, ___,                  ___, ___, ___, ___, ___, ___, ___, ___,
                            ___, ___, ___, ___, ___,             ___, ___, ___, ___, ___
    ),
    [MEDIA] = LAYOUT(
        ___, ___, ___, ___, ___, ___,                            ___, ___, ___, ___, ___, ___,
        ___, ___, ___, ___, ___, ___,                            ___, ___, ___, ___, ___, ___,
        ___, KC_MPRV, KC_MPLY, KC_MNXT, KC_MUTE, ___,           ___, KC_VOLD, KC_VOLU, ___, ___, ___,
        ___, ___, ___, ___, ___, ___, ___, ___,                  ___, ___, ___, ___, ___, ___, ___, ___,
                            ___, ___, ___, ___, ___,             ___, ___, ___, ___, ___
    ),
    [MOUSE] = LAYOUT(
        ___, ___, ___, ___, ___, ___,                            ___, ___, ___, ___, ___, ___,
        ___, ___, ___, ___, ___, ___,                            ___, ___, MS_UP, ___, ___, ___,
        ___, ___, ___, ___, ___, ___,                            ___, MS_LEFT, MS_DOWN, MS_RGHT, ___, ___,
        ___, ___, ___, ___, ___, ___, ___, ___,                  ___, ___, MS_BTN1, MS_BTN2, MS_BTN3, ___, ___, ___,
                            ___, ___, ___, ___, ___,             ___, ___, ___, ___, ___
    ),
};

bool encoder_update_user(uint8_t index, bool clockwise) {
    (void)index;
    tap_code16(clockwise ? KC_TAB : S(KC_TAB));
    return false;
}

#ifdef HALCYON_ENABLE
// The module's push switch stays unassigned until a behavior is chosen.
const uint16_t left_halcyon_buttons[10][5] = {{KC_NO}};
const uint16_t right_halcyon_buttons[10][5] = {{KC_NO}};
#endif
