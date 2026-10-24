// SPDX-License-Identifier: GPL-2.0-or-later
#include QMK_KEYBOARD_H
#include "split_util.h"
#include "hlc_tft_display/hlc_tft_display.h"
#include "hlc_tft_display/graphics/fonts/Retron2000-27.qff.h"
#include "smiley.h"
#include "tap_hold.h"
#include "keylog.h"
#include <string.h>

static painter_font_handle_t status_font;
static char last_keys[2][9] = {"-", "-"};
static const char *const layer_names[] = {
    [BASE] = "BASE", [NAV] = "NAV", [SYM] = "SYM", [NUM] = "NUM",
    [FUN] = "FUN", [MEDIA] = "MEDIA", [MOUSE] = "MOUSE",
};

enum {
    // Inactivity in milliseconds before the key display changes to the idle animation.
    IDLE_DELAY_MS = 3000,
    // Startup grace period in milliseconds before reporting a disconnected split.
    SPLIT_GRACE_MS = 3000,
    // Minimum interval in milliseconds between display redraws.
    REDRAW_THROTTLE_MS = 30,
};

bool module_post_init_user(void) {
    status_font = qp_load_font_mem(font_Retron2000_27);
    return false;
}

bool display_module_housekeeping_task_user(bool second_display) {
    (void)second_display;
    static uint8_t last_step = 255;
    static uint8_t last_layer = 255;
    static bool was_idle = false;
    static bool last_split_lost = false;
    static uint8_t last_mods = 0xff;
    static uint32_t last_draw_ms = 0;

    // SplitKB's SPLIT_ACTIVITY_ENABLE synchronizes input activity to the
    // display half, including keys pressed on the other half.
    const keylog_state_t *keylog = keylog_get_state();
    bool keys_changed = memcmp(last_keys, keylog->labels, sizeof(last_keys)) != 0;
    uint32_t idle_elapsed_ms = last_input_activity_elapsed();
    uint8_t mods = get_mods() | get_weak_mods() | get_oneshot_mods();
    bool idle = idle_elapsed_ms >= IDLE_DELAY_MS && mods == 0;
    bool split_lost = timer_read32() >= SPLIT_GRACE_MS && !is_transport_connected();
    uint8_t layer = get_highest_layer(layer_state | default_layer_state);
    uint8_t step = idle ? ((idle_elapsed_ms - IDLE_DELAY_MS) / SMILEY_STEP_MS) % smiley_step_count() : 0;
    if (idle && was_idle && step == last_step && layer == last_layer && split_lost == last_split_lost) {
        return false;
    }
    if (!idle && !was_idle && !keys_changed && mods == last_mods && layer == last_layer && split_lost == last_split_lost) {
        return false;
    }
    if (timer_elapsed32(last_draw_ms) < REDRAW_THROTTLE_MS) {
        return false;
    }
    last_draw_ms = timer_read32();
    last_step = step;
    last_layer = layer;
    was_idle = idle;
    last_split_lost = split_lost;
    last_mods = mods;
    memcpy(last_keys, keylog->labels, sizeof(last_keys));

    qp_rect(lcd_surface, 0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1, HSV_BLACK, true);
    if (idle) {
        draw_smiley_step(lcd_surface, step, (LCD_WIDTH - 80) / 2, (LCD_HEIGHT - 80) / 2);
    } else if (status_font) {
        char line[12];
        strcpy(line, "L: ");
        strcat(line, last_keys[0]);
        qp_drawtext_recolor(lcd_surface, 4, 48, status_font, line, HSV_WHITE, HSV_BLACK);
        strcpy(line, "R: ");
        strcat(line, last_keys[1]);
        qp_drawtext_recolor(lcd_surface, 4, 86, status_font, line, HSV_WHITE, HSV_BLACK);
        uint8_t y = 124;
        if (mods & MOD_MASK_CTRL) {
            qp_drawtext_recolor(lcd_surface, 4, y, status_font, "CTRL", HSV_WHITE, HSV_BLACK);
            y += 28;
        }
        if (mods & MOD_MASK_SHIFT) {
            qp_drawtext_recolor(lcd_surface, 4, y, status_font, "SHIFT", HSV_WHITE, HSV_BLACK);
            y += 28;
        }
        if (mods & MOD_MASK_ALT) {
            qp_drawtext_recolor(lcd_surface, 4, y, status_font, "ALT", HSV_WHITE, HSV_BLACK);
            y += 28;
        }
        if (mods & MOD_MASK_GUI) {
            qp_drawtext_recolor(lcd_surface, 4, y, status_font, "CMD/WIN", HSV_WHITE, HSV_BLACK);
        }
    }
    if (split_lost && status_font) {
        qp_drawtext_recolor(lcd_surface, 4, 4, status_font, "SPLIT", HSV_RED, HSV_BLACK);
    }
    if (layer != BASE && !split_lost && status_font) {
        const char *name = layer < sizeof(layer_names) / sizeof(layer_names[0]) ? layer_names[layer] : NULL;
        qp_drawtext_recolor(lcd_surface, 4, 4, status_font, name ? name : "LAYER", HSV_WHITE, HSV_BLACK);
    }
    qp_surface_draw(lcd_surface, lcd, 0, 0, 0);
    qp_flush(lcd);
    return false;
}
