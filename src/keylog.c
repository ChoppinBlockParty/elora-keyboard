// SPDX-License-Identifier: GPL-2.0-or-later
#include "quantum.h"
#include "keylog.h"
#include "tap_hold.h"
#include "split_util.h"
#include "transactions.h"
#include <string.h>

static keylog_state_t state = {.labels = {"-", "-"}};

enum {
    // Minimum interval in milliseconds between split sync attempts, including retries.
    SYNC_INTERVAL_MS = 50,
    // Refresh interval in milliseconds to restore labels after the display restarts.
    SYNC_REFRESH_MS = 1000,
};

const keylog_state_t *keylog_get_state(void) {
    return &state;
}

// Matrix rows from the right half follow the left half's rows on the master.
void keylog_note_keypress(uint16_t keycode, keyrecord_t *record) {
    if (!record->event.pressed || keycode == KC_NO || keycode == KC_TRNS) {
        return;
    }

    char *label = state.labels[record->event.key.row >= MATRIX_ROWS / 2];
    if (IS_QK_MOD_TAP(keycode) || IS_QK_LAYER_TAP(keycode)) {
        if (record->tap.count == 0) {
            // A held layer or modifier key has no typed character.
            strcpy(label, IS_QK_LAYER_TAP(keycode) ? "LAYER" : "MOD");
            return;
        }
        keycode &= 0xFF;
    }

    if (keycode >= KC_A && keycode <= KC_Z) {
        label[0] = 'A' + keycode - KC_A;
        label[1] = '\0';
    } else if (keycode >= KC_1 && keycode <= KC_9) {
        label[0] = '1' + keycode - KC_1;
        label[1] = '\0';
    } else if (keycode >= KC_F1 && keycode <= KC_F12) {
        label[0] = 'F';
        if (keycode <= KC_F9) {
            label[1] = '1' + keycode - KC_F1;
            label[2] = '\0';
        } else {
            label[1] = '1';
            label[2] = '0' + keycode - KC_F10;
            label[3] = '\0';
        }
    } else {
        const char *name = "KEY";
        switch (keycode) {
            case KC_0: name = "0"; break;
            case KC_ESC: name = "ESC"; break;
            case KC_TAB: name = "TAB"; break;
            case KC_SPC: name = "SPACE"; break;
            case KC_ENT: name = "ENTER"; break;
            case KC_BSPC: name = "BSPC"; break;
            case KC_DEL: name = "DEL"; break;
            case KC_COMM: name = ","; break;
            case KC_DOT: name = "."; break;
            case KC_SLSH: name = "/"; break;
            case KC_SCLN: name = ";"; break;
            case KC_QUOT: name = "'"; break;
            case KC_GRV: name = "`"; break;
            case KC_MINS: name = "-"; break;
            case KC_EQL: name = "="; break;
            case KC_BSLS: name = "\\"; break;
            case KC_LBRC: name = "["; break;
            case KC_RBRC: name = "]"; break;
            case KC_TILD: name = "~"; break;
            case KC_EXLM: name = "!"; break;
            case KC_AT: name = "@"; break;
            case KC_HASH: name = "#"; break;
            case KC_DLR: name = "$"; break;
            case KC_PERC: name = "%"; break;
            case KC_CIRC: name = "^"; break;
            case KC_AMPR: name = "&"; break;
            case KC_ASTR: name = "*"; break;
            case KC_LPRN: name = "("; break;
            case KC_RPRN: name = ")"; break;
            case KC_LCBR: name = "{"; break;
            case KC_RCBR: name = "}"; break;
            case KC_PIPE: name = "|"; break;
            case KC_HOME: name = "HOME"; break;
            case KC_END: name = "END"; break;
            case KC_PGUP: name = "PGUP"; break;
            case KC_PGDN: name = "PGDN"; break;
            case KC_UP: name = "UP"; break;
            case KC_DOWN: name = "DOWN"; break;
            case KC_LEFT: name = "LEFT"; break;
            case KC_RGHT: name = "RIGHT"; break;
            case KC_LSFT: case KC_RSFT: name = "SHIFT"; break;
            case KC_LCTL: case KC_RCTL: name = "CTRL"; break;
            case KC_LALT: case KC_RALT: name = "ALT"; break;
            case KC_LGUI: case KC_RGUI: name = "GUI"; break;
            case KC_MPRV: name = "PREV"; break;
            case KC_MNXT: name = "NEXT"; break;
            case KC_MPLY: name = "PLAY"; break;
            case KC_VOLD: name = "VOL-"; break;
            case KC_VOLU: name = "VOL+"; break;
            case KC_MUTE: name = "MUTE"; break;
            case MS_UP: name = "M-UP"; break;
            case MS_DOWN: name = "M-DN"; break;
            case MS_LEFT: name = "M-LT"; break;
            case MS_RGHT: name = "M-RT"; break;
            case MS_BTN1: name = "MB1"; break;
            case MS_BTN2: name = "MB2"; break;
            case MS_BTN3: name = "MB3"; break;
        }
        strcpy(label, name);
    }
}

static void receive_keylog(uint8_t in_size, const void *in_data, uint8_t out_size, void *out_data) {
    (void)out_size;
    (void)out_data;
    if (in_size == sizeof(state)) {
        memcpy(&state, in_data, sizeof(state));
    }
}

void keyboard_post_init_user(void) {
    transaction_register_rpc(ELORA_KEYLOG_SYNC, receive_keylog);
}

void housekeeping_task_user(void) {
    // The left TFT can read local state when it is the USB master. When the
    // right half is master, send both labels to the display on the left.
    if (!is_keyboard_master() || is_keyboard_left() || !is_transport_connected()) {
        return;
    }

    static keylog_state_t last_sent;
    static uint32_t last_attempt_ms;
    static uint32_t last_success_ms;
    if (timer_elapsed32(last_attempt_ms) < SYNC_INTERVAL_MS) {
        return;
    }
    // Refresh unchanged labels periodically so a restarted slave recovers.
    if (memcmp(&state, &last_sent, sizeof(state)) == 0 && timer_elapsed32(last_success_ms) < SYNC_REFRESH_MS) {
        return;
    }
    last_attempt_ms = timer_read32();
    if (transaction_rpc_send(ELORA_KEYLOG_SYNC, sizeof(state), &state)) {
        last_sent = state;
        last_success_ms = timer_read32();
    }
}
