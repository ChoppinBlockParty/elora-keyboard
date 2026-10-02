// SPDX-License-Identifier: GPL-2.0-or-later
#include QMK_KEYBOARD_H
#include "split_util.h"
#include "hlc_tft_display/hlc_tft_display.h"
#include "hlc_tft_display/graphics/fonts/Retron2000-27.qff.h"
#include "smiley.h"

static painter_font_handle_t status_font;

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
    static uint32_t last_draw = 0;

    // SplitKB's SPLIT_ACTIVITY_ENABLE synchronizes input activity to the
    // display half, including keys pressed on the other half.
    uint32_t idle_elapsed = last_input_activity_elapsed();
    bool idle = idle_elapsed >= 3000;
    bool split_lost = timer_read32() >= 3000 && !is_transport_connected();
    uint8_t layer = get_highest_layer(layer_state | default_layer_state);
    uint8_t step = idle ? ((idle_elapsed - 3000) / SMILEY_STEP_MS) % smiley_step_count() : 0;
    if (idle && was_idle && step == last_step && layer == last_layer && split_lost == last_split_lost) {
        return false;
    }
    if (!idle && !was_idle && layer == last_layer && split_lost == last_split_lost) {
        return false;
    }
    if (timer_elapsed32(last_draw) < 30) {
        return false;
    }
    last_draw = timer_read32();
    last_step = step;
    last_layer = layer;
    was_idle = idle;
    last_split_lost = split_lost;

    qp_rect(lcd_surface, 0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1, HSV_BLACK, true);
    if (idle) {
        draw_smiley_step(lcd_surface, step, (LCD_WIDTH - 80) / 2, (LCD_HEIGHT - 80) / 2);
    }
    if (split_lost && status_font) {
        qp_drawtext_recolor(lcd_surface, 4, 4, status_font, "SPLIT", HSV_RED, HSV_BLACK);
    }
    if (layer != 0 && !split_lost && status_font) {
        static const char *const names[] = {"", "NAV", "SYM", "NUM", "FUN", "MEDIA", "MOUSE"};
        qp_drawtext_recolor(lcd_surface, 4, 4, status_font, layer < 7 ? names[layer] : "LAYER", HSV_WHITE, HSV_BLACK);
    }
    qp_surface_draw(lcd_surface, lcd, 0, 0, 0);
    return false;
}
