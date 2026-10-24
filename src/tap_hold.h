// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include "quantum.h"

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
