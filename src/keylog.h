// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include "quantum.h"

typedef struct {
    char labels[2][9];
} keylog_state_t;

const keylog_state_t *keylog_get_state(void);
void keylog_note_keypress(uint16_t keycode, keyrecord_t *record);
