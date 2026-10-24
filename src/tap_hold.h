// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include "quantum.h"

enum layers {
    BASE,
    NAV,
    SYM,
    NUM,
    FUN,
    MEDIA,
    MOUSE,
};

#define A_GUI     LGUI_T(KC_A)
#define S_ALT     LALT_T(KC_S)
#define D_CTRL    LCTL_T(KC_D)
#define F_SHIFT   LSFT_T(KC_F)
#define J_SHIFT   RSFT_T(KC_J)
#define K_CTRL    RCTL_T(KC_K)
#define L_ALT     RALT_T(KC_L)
#define SCLN_GUI  RGUI_T(KC_SCLN)
#define ESC_CTRL  LCTL_T(KC_ESC)
#define QUOT_CTRL RCTL_T(KC_QUOT)

#define ESC_MEDIA LT(MEDIA, KC_ESC)
#define SPC_NAV   LT(NAV, KC_SPC)
#define TAB_MOUSE LT(MOUSE, KC_TAB)
#define ENT_SYM   LT(SYM, KC_ENT)
#define BSPC_NUM  LT(NUM, KC_BSPC)
#define DEL_FUN   LT(FUN, KC_DEL)
