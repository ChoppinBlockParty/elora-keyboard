// SPDX-License-Identifier: GPL-2.0-or-later
#include QMK_KEYBOARD_H
#include "split_util.h"
#include "hlc_tft_display/hlc_tft_display.h"
#include "hlc_tft_display/graphics/fonts/Retron2000-27.qff.h"
#include "smiley.h"
#include "tap_hold.h"
#include "keylog.h"
#include <stdio.h>
#include <string.h>

static painter_font_handle_t status_font;
static keylog_state_t last_drawn = {.labels = {"-", "-"}};
static const char *const layer_names[] = {
    [BASE] = "BASE",
    [NAV] = "NAV",
    [SYM] = "SYM",
    [NUM] = "NUM",
    [FUN] = "FUN",
    [MEDIA] = "MEDIA",
    [MOUSE] = "MOUSE",
};

enum {
    IDLE_DELAY_MS = 3000,
    SPLIT_GRACE_MS = 3000,
    REDRAW_THROTTLE_MS = 30,
    TEXT_MARGIN_PX = 4,
    NOTICE_ROW_PX = 4,
    LEFT_KEY_ROW_PX = 48,
    RIGHT_KEY_ROW_PX = 86,
    MODIFIER_FIRST_ROW_PX = 124,
    // Matches the 27 px font's line height.
    MODIFIER_ROW_PITCH_PX = 28,
};

#define KEY_LINE_PREFIX_LEN 3
#define KEY_LINE_CAPACITY   (KEY_LINE_PREFIX_LEN + sizeof(last_drawn.labels[0]))

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

    // SPLIT_ACTIVITY_ENABLE makes this half's activity timer count the other
    // half's keys too, so idle here means idle on both.
    const keylog_state_t *keylog = keylog_get_state();
    bool keys_changed = memcmp(&last_drawn, keylog, sizeof(last_drawn)) != 0;
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
    memcpy(&last_drawn, keylog, sizeof(last_drawn));

    qp_rect(lcd_surface, 0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1, HSV_BLACK, true);
    if (idle) {
        draw_smiley_step(lcd_surface, step, (LCD_WIDTH - SMILEY_SIZE_PX) / 2, (LCD_HEIGHT - SMILEY_SIZE_PX) / 2);
    } else if (status_font) {
        char line[KEY_LINE_CAPACITY];
        (void)snprintf(line, sizeof(line), "L: %s", last_drawn.labels[0]);
        qp_drawtext_recolor(lcd_surface, TEXT_MARGIN_PX, LEFT_KEY_ROW_PX, status_font, line, HSV_WHITE, HSV_BLACK);
        (void)snprintf(line, sizeof(line), "R: %s", last_drawn.labels[1]);
        qp_drawtext_recolor(lcd_surface, TEXT_MARGIN_PX, RIGHT_KEY_ROW_PX, status_font, line, HSV_WHITE, HSV_BLACK);
        uint8_t y = MODIFIER_FIRST_ROW_PX;
        if (mods & MOD_MASK_CTRL) {
            qp_drawtext_recolor(lcd_surface, TEXT_MARGIN_PX, y, status_font, "CTRL", HSV_WHITE, HSV_BLACK);
            y += MODIFIER_ROW_PITCH_PX;
        }
        if (mods & MOD_MASK_SHIFT) {
            qp_drawtext_recolor(lcd_surface, TEXT_MARGIN_PX, y, status_font, "SHIFT", HSV_WHITE, HSV_BLACK);
            y += MODIFIER_ROW_PITCH_PX;
        }
        if (mods & MOD_MASK_ALT) {
            qp_drawtext_recolor(lcd_surface, TEXT_MARGIN_PX, y, status_font, "ALT", HSV_WHITE, HSV_BLACK);
            y += MODIFIER_ROW_PITCH_PX;
        }
        if (mods & MOD_MASK_GUI) {
            qp_drawtext_recolor(lcd_surface, TEXT_MARGIN_PX, y, status_font, "CMD/WIN", HSV_WHITE, HSV_BLACK);
        }
    }
    if (split_lost && status_font) {
        qp_drawtext_recolor(lcd_surface, TEXT_MARGIN_PX, NOTICE_ROW_PX, status_font, "SPLIT", HSV_RED, HSV_BLACK);
    }
    if (layer != BASE && !split_lost && status_font) {
        const char *name = layer < sizeof(layer_names) / sizeof(layer_names[0]) ? layer_names[layer] : NULL;
        qp_drawtext_recolor(lcd_surface, TEXT_MARGIN_PX, NOTICE_ROW_PX, status_font, name ? name : "LAYER", HSV_WHITE, HSV_BLACK);
    }
    qp_surface_draw(lcd_surface, lcd, 0, 0, 0);
    qp_flush(lcd);
    return false;
}
