#include "keyboard_report_util.hpp"
#include "test_common.hpp"
#include "test_fixture.hpp"
#include "test_keymap_key.hpp"
#include <algorithm>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

extern "C" {
#include "tap_hold.h"
}

using testing::InSequence;
using testing::_;

class EloraTapHold : public TestFixture {};

TEST_F(EloraTapHold, same_hand_shift_digit_nested_tap) {
    TestDriver driver;
    InSequence order;
    auto shift_f = KeymapKey(BASE, 2, 1, F_SHIFT);
    auto five = KeymapKey(BASE, 3, 0, KC_5);
    set_keymap({shift_f, five});

    EXPECT_NO_REPORT(driver);
    shift_f.press();
    run_one_scan_loop();
    EXPECT_REPORT(driver, (KC_LEFT_SHIFT));
    EXPECT_REPORT(driver, (KC_LEFT_SHIFT, KC_5));
    EXPECT_REPORT(driver, (KC_LEFT_SHIFT));
    tap_key(five);
    VERIFY_AND_CLEAR(driver);
    EXPECT_EMPTY_REPORT(driver);
    shift_f.release();
    run_one_scan_loop();
    VERIFY_AND_CLEAR(driver);
}

TEST_F(EloraTapHold, same_hand_letter_roll_stays_taps) {
    TestDriver driver;
    InSequence order;
    auto shift_f = KeymapKey(BASE, 2, 1, F_SHIFT);
    auto v = KeymapKey(BASE, 3, 2, KC_V);
    set_keymap({shift_f, v});

    EXPECT_NO_REPORT(driver);
    shift_f.press();
    run_one_scan_loop();
    v.press();
    run_one_scan_loop();
    VERIFY_AND_CLEAR(driver);
    EXPECT_REPORT(driver, (KC_F));
    EXPECT_REPORT(driver, (KC_F, KC_V));
    EXPECT_REPORT(driver, (KC_V));
    shift_f.release();
    run_one_scan_loop();
    VERIFY_AND_CLEAR(driver);
    EXPECT_EMPTY_REPORT(driver);
    v.release();
    run_one_scan_loop();
    VERIFY_AND_CLEAR(driver);
}

TEST_F(EloraTapHold, same_hand_shift_digit_roll_stays_taps) {
    TestDriver driver;
    InSequence order;
    auto shift_f = KeymapKey(BASE, 2, 1, F_SHIFT);
    auto five = KeymapKey(BASE, 3, 0, KC_5);
    set_keymap({shift_f, five});

    EXPECT_NO_REPORT(driver);
    shift_f.press();
    run_one_scan_loop();
    five.press();
    run_one_scan_loop();
    VERIFY_AND_CLEAR(driver);
    EXPECT_REPORT(driver, (KC_F));
    EXPECT_REPORT(driver, (KC_F, KC_5));
    EXPECT_REPORT(driver, (KC_5));
    shift_f.release();
    run_one_scan_loop();
    VERIFY_AND_CLEAR(driver);
    EXPECT_EMPTY_REPORT(driver);
    five.release();
    run_one_scan_loop();
    VERIFY_AND_CLEAR(driver);
}

TEST_F(EloraTapHold, opposite_hand_nav_chord_activates_immediately) {
    TestDriver driver;
    InSequence order;
    auto nav_thumb = KeymapKey(BASE, 2, 3, SPC_NAV);
    auto j_base = KeymapKey(BASE, 6, 1, J_SHIFT);
    auto j_nav = KeymapKey(NAV, 6, 1, KC_DOWN);
    set_keymap({nav_thumb, j_base, j_nav});

    EXPECT_NO_REPORT(driver);
    nav_thumb.press();
    run_one_scan_loop();
    EXPECT_REPORT(driver, (KC_DOWN));
    EXPECT_EMPTY_REPORT(driver);
    tap_key(j_base);
    VERIFY_AND_CLEAR(driver);
    nav_thumb.release();
    run_one_scan_loop();
    VERIFY_AND_CLEAR(driver);
}

TEST_F(EloraTapHold, same_hand_nav_chord_stays_space_and_letter) {
    TestDriver driver;
    InSequence order;
    auto nav_thumb = KeymapKey(BASE, 2, 3, SPC_NAV);
    auto w = KeymapKey(BASE, 1, 0, KC_W);
    set_keymap({nav_thumb, w});

    EXPECT_NO_REPORT(driver);
    nav_thumb.press();
    run_one_scan_loop();
    EXPECT_REPORT(driver, (KC_SPACE));
    EXPECT_REPORT(driver, (KC_SPACE, KC_W));
    EXPECT_REPORT(driver, (KC_SPACE));
    tap_key(w);
    VERIFY_AND_CLEAR(driver);
    EXPECT_EMPTY_REPORT(driver);
    nav_thumb.release();
    run_one_scan_loop();
    VERIFY_AND_CLEAR(driver);
}

TEST_F(EloraTapHold, thumb_tap_then_immediate_hold_selects_layer) {
    TestDriver driver;
    InSequence order;
    auto nav_thumb = KeymapKey(BASE, 2, 3, SPC_NAV);
    set_keymap({nav_thumb});

    EXPECT_REPORT(driver, (KC_SPACE));
    EXPECT_EMPTY_REPORT(driver);
    tap_key(nav_thumb);
    VERIFY_AND_CLEAR(driver);

    EXPECT_NO_REPORT(driver);
    nav_thumb.press();
    idle_for(TAPPING_TERM + 1);
    VERIFY_AND_CLEAR(driver);
    expect_layer_state(NAV);
    nav_thumb.release();
    run_one_scan_loop();
    expect_layer_state(BASE);
}

TEST_F(EloraTapHold, overlapping_nav_and_num_resolves_highest_layer) {
    TestDriver driver;
    InSequence order;
    auto nav_thumb = KeymapKey(BASE, 2, 3, SPC_NAV);
    auto num_thumb = KeymapKey(BASE, 7, 3, BSPC_NUM);
    auto w_base = KeymapKey(BASE, 1, 0, KC_W);
    auto num_thumb_nav = KeymapKey(NAV, 7, 3, KC_TRNS);
    auto w_nav = KeymapKey(NAV, 1, 0, KC_TRNS);
    auto w_num = KeymapKey(NUM, 1, 0, KC_7);
    set_keymap({nav_thumb, num_thumb, w_base, num_thumb_nav, w_nav, w_num});

    EXPECT_NO_REPORT(driver);
    nav_thumb.press();
    run_one_scan_loop();
    num_thumb.press();
    run_one_scan_loop();
    VERIFY_AND_CLEAR(driver);

    EXPECT_REPORT(driver, (KC_7));
    EXPECT_EMPTY_REPORT(driver);
    tap_key(w_base);
    VERIFY_AND_CLEAR(driver);
    num_thumb.release();
    run_one_scan_loop();
    nav_thumb.release();
    run_one_scan_loop();
    VERIFY_AND_CLEAR(driver);
}

TEST_F(EloraTapHold, same_hand_shift_ctrl_digit_nested_tap) {
    TestDriver driver;
    InSequence order;
    auto shift_f = KeymapKey(BASE, 2, 1, F_SHIFT);
    auto ctrl_d = KeymapKey(BASE, 1, 1, D_CTRL);
    auto five = KeymapKey(BASE, 3, 0, KC_5);
    set_keymap({shift_f, ctrl_d, five});

    EXPECT_NO_REPORT(driver);
    shift_f.press();
    run_one_scan_loop();
    ctrl_d.press();
    run_one_scan_loop();
    EXPECT_REPORT(driver, (KC_LEFT_SHIFT));
    EXPECT_REPORT(driver, (KC_LEFT_SHIFT, KC_LEFT_CTRL));
    EXPECT_REPORT(driver, (KC_LEFT_SHIFT, KC_LEFT_CTRL, KC_5));
    EXPECT_REPORT(driver, (KC_LEFT_SHIFT, KC_LEFT_CTRL));
    tap_key(five);
    VERIFY_AND_CLEAR(driver);
    EXPECT_REPORT(driver, (KC_LEFT_SHIFT));
    ctrl_d.release();
    run_one_scan_loop();
    VERIFY_AND_CLEAR(driver);
    EXPECT_EMPTY_REPORT(driver);
    shift_f.release();
    run_one_scan_loop();
    VERIFY_AND_CLEAR(driver);
}

TEST_F(EloraTapHold, held_j_shifts_repeated_semicolons) {
    TestDriver driver;
    InSequence order;
    auto shift_j = KeymapKey(BASE, 6, 1, J_SHIFT);
    auto semicolon = KeymapKey(BASE, 9, 1, SCLN_GUI);
    set_keymap({shift_j, semicolon});

    EXPECT_NO_REPORT(driver);
    shift_j.press();
    run_one_scan_loop();
    for (int i = 0; i < 3; ++i) {
        if (i == 0) {
            EXPECT_REPORT(driver, (KC_RIGHT_SHIFT));
        }
        EXPECT_REPORT(driver, (KC_RIGHT_SHIFT, KC_SCLN));
        EXPECT_REPORT(driver, (KC_RIGHT_SHIFT));
        tap_key(semicolon);
        VERIFY_AND_CLEAR(driver);
    }
    EXPECT_EMPTY_REPORT(driver);
    shift_j.release();
    run_one_scan_loop();
    VERIFY_AND_CLEAR(driver);
}

TEST_F(EloraTapHold, rolling_j_semicolon_stays_two_taps) {
    TestDriver driver;
    InSequence order;
    auto shift_j = KeymapKey(BASE, 6, 1, J_SHIFT);
    auto semicolon = KeymapKey(BASE, 9, 1, SCLN_GUI);
    set_keymap({shift_j, semicolon});

    EXPECT_NO_REPORT(driver);
    shift_j.press();
    run_one_scan_loop();
    semicolon.press();
    run_one_scan_loop();
    VERIFY_AND_CLEAR(driver);
    EXPECT_REPORT(driver, (KC_J));
    EXPECT_EMPTY_REPORT(driver);
    shift_j.release();
    run_one_scan_loop();
    VERIFY_AND_CLEAR(driver);
    EXPECT_REPORT(driver, (KC_SCLN));
    EXPECT_EMPTY_REPORT(driver);
    semicolon.release();
    run_one_scan_loop();
    VERIFY_AND_CLEAR(driver);
}

struct HomeModSpec {
    uint16_t keycode;
    uint8_t tap;
    uint8_t modifier;
    uint8_t column;
};

static const HomeModSpec home_mods[] = {
    {ESC_CTRL, KC_ESC, KC_LEFT_CTRL, 0},
    {A_GUI, KC_A, KC_LEFT_GUI, 1},
    {S_ALT, KC_S, KC_LEFT_ALT, 2},
    {D_CTRL, KC_D, KC_LEFT_CTRL, 3},
    {F_SHIFT, KC_F, KC_LEFT_SHIFT, 4},
    {J_SHIFT, KC_J, KC_RIGHT_SHIFT, 5},
    {K_CTRL, KC_K, KC_RIGHT_CTRL, 6},
    {L_ALT, KC_L, KC_RIGHT_ALT, 7},
    {SCLN_GUI, KC_SCLN, KC_RIGHT_GUI, 8},
    {QUOT_CTRL, KC_QUOT, KC_RIGHT_CTRL, 9},
};

static const uint8_t plain_keys[] = {
    KC_A, KC_B, KC_C, KC_D, KC_E, KC_F, KC_G, KC_H, KC_I,
    KC_J, KC_K, KC_L, KC_M, KC_N, KC_O, KC_P, KC_Q, KC_R,
    KC_S, KC_T, KC_U, KC_V, KC_W, KC_X, KC_Y, KC_Z,
    KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0,
    KC_SCLN, KC_QUOT, KC_COMM, KC_DOT, KC_SLSH,
    KC_UP, KC_DOWN, KC_LEFT, KC_RGHT,
    KC_ESC, KC_TAB, KC_SPC, KC_ENT, KC_BSPC, KC_DEL,
};

TEST_F(EloraTapHold, every_modifier_chords_with_plain_keys_on_either_hand) {
    TestDriver driver;
    InSequence order;

    for (const auto& mod : home_mods) {
        for (uint8_t second_code : plain_keys) {
            for (bool same_hand : {false, true}) {
                SCOPED_TRACE(std::string(get_keycode_string(mod.keycode)) + "+" + get_keycode_string(second_code) + (same_hand ? " same hand" : " opposite hand"));
                const uint8_t second_column = (mod.column < 5) == same_hand ? 4 : 5;
                auto first = KeymapKey(BASE, mod.column, 1, mod.keycode);
                auto second = KeymapKey(BASE, second_column, 0, second_code);
                set_keymap({first, second});

                EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(std::vector<uint8_t>{mod.modifier})));
                EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(std::vector<uint8_t>{mod.modifier, second_code})));
                EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(std::vector<uint8_t>{mod.modifier})));
                EXPECT_EMPTY_REPORT(driver);
                first.press();
                run_one_scan_loop();
                tap_key(second);
                first.release();
                run_one_scan_loop();
                VERIFY_AND_CLEAR(driver);
                idle_for(TAPPING_TERM + 1);
            }
        }
    }
}

TEST_F(EloraTapHold, every_modifier_rolls_into_plain_keys_on_either_hand) {
    TestDriver driver;
    InSequence order;
    for (const auto& mod : home_mods) {
        for (uint8_t second_code : plain_keys) {
            if (second_code == mod.tap) {
                continue;
            }
            for (bool same_hand : {false, true}) {
                SCOPED_TRACE(std::string(get_keycode_string(mod.keycode)) + "+" + get_keycode_string(second_code) + (same_hand ? " same hand" : " opposite hand"));
                const uint8_t second_column = (mod.column < 5) == same_hand ? 4 : 5;
                auto first = KeymapKey(BASE, mod.column, 1, mod.keycode);
                auto second = KeymapKey(BASE, second_column, 0, second_code);
                set_keymap({first, second});

                EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(std::vector<uint8_t>{mod.tap})));
                EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(std::vector<uint8_t>{mod.tap, second_code})));
                EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(std::vector<uint8_t>{second_code})));
                EXPECT_EMPTY_REPORT(driver);
                first.press();
                run_one_scan_loop();
                second.press();
                run_one_scan_loop();
                first.release();
                run_one_scan_loop();
                second.release();
                run_one_scan_loop();
                VERIFY_AND_CLEAR(driver);
                idle_for(TAPPING_TERM + 1);
            }
        }
    }
}

TEST_F(EloraTapHold, every_modifier_chords_with_other_home_mod_taps) {
    TestDriver driver;
    InSequence order;
    for (const auto& mod : home_mods) {
        for (const auto& other : home_mods) {
            if (mod.column == other.column) {
                continue;
            }
            SCOPED_TRACE(std::string(get_keycode_string(mod.keycode)) + "+" + get_keycode_string(other.keycode));
            auto first = KeymapKey(BASE, mod.column, 1, mod.keycode);
            auto second = KeymapKey(BASE, other.column, 1, other.keycode);
            set_keymap({first, second});

            EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(std::vector<uint8_t>{mod.modifier})));
            EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(std::vector<uint8_t>{mod.modifier, other.tap})));
            EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(std::vector<uint8_t>{mod.modifier})));
            EXPECT_EMPTY_REPORT(driver);
            first.press();
            run_one_scan_loop();
            tap_key(second);
            first.release();
            run_one_scan_loop();
            VERIFY_AND_CLEAR(driver);
            idle_for(TAPPING_TERM + 1);
        }
    }
}

TEST_F(EloraTapHold, every_modifier_rolls_into_other_home_mod_taps) {
    TestDriver driver;
    InSequence order;
    for (const auto& mod : home_mods) {
        for (const auto& other : home_mods) {
            if (mod.column == other.column) {
                continue;
            }
            SCOPED_TRACE(std::string(get_keycode_string(mod.keycode)) + "+" + get_keycode_string(other.keycode));
            auto first = KeymapKey(BASE, mod.column, 1, mod.keycode);
            auto second = KeymapKey(BASE, other.column, 1, other.keycode);
            set_keymap({first, second});

            EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(std::vector<uint8_t>{mod.tap})));
            EXPECT_EMPTY_REPORT(driver);
            EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(std::vector<uint8_t>{other.tap})));
            EXPECT_EMPTY_REPORT(driver);
            first.press();
            run_one_scan_loop();
            second.press();
            run_one_scan_loop();
            first.release();
            run_one_scan_loop();
            second.release();
            run_one_scan_loop();
            VERIFY_AND_CLEAR(driver);
            idle_for(TAPPING_TERM + 1);
        }
    }
}

TEST_F(EloraTapHold, every_modifier_chords_with_each_layer_thumb_tap) {
    TestDriver driver;
    InSequence order;
    struct ThumbSpec {
        uint16_t keycode;
        uint8_t tap;
        uint8_t column;
    };
    const ThumbSpec thumbs[] = {
        {ESC_MEDIA, KC_ESC, 1}, {SPC_NAV, KC_SPC, 2}, {TAB_MOUSE, KC_TAB, 3},
        {ENT_SYM, KC_ENT, 6}, {BSPC_NUM, KC_BSPC, 7}, {DEL_FUN, KC_DEL, 8},
    };
    for (const auto& mod : home_mods) {
        for (const auto& thumb : thumbs) {
            SCOPED_TRACE(std::string(get_keycode_string(mod.keycode)) + "+" + get_keycode_string(thumb.keycode));
            auto first = KeymapKey(BASE, mod.column, 1, mod.keycode);
            auto second = KeymapKey(BASE, thumb.column, 3, thumb.keycode);
            set_keymap({first, second});

            EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(std::vector<uint8_t>{mod.modifier})));
            EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(std::vector<uint8_t>{mod.modifier, thumb.tap})));
            EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(std::vector<uint8_t>{mod.modifier})));
            EXPECT_EMPTY_REPORT(driver);
            first.press();
            run_one_scan_loop();
            tap_key(second);
            first.release();
            run_one_scan_loop();
            VERIFY_AND_CLEAR(driver);
            idle_for(TAPPING_TERM + 1);
        }
    }
}

TEST_F(EloraTapHold, every_modifier_works_near_and_past_tapping_term) {
    TestDriver driver;
    InSequence order;
    for (const auto& mod : home_mods) {
        for (bool same_hand : {false, true}) {
            for (unsigned delay_ms : {190u, 210u}) {
                SCOPED_TRACE(std::string(get_keycode_string(mod.keycode)) + (same_hand ? " same hand" : " opposite hand") + " delay " + std::to_string(delay_ms));
                const uint8_t second_column = (mod.column < 5) == same_hand ? 4 : 5;
                auto first = KeymapKey(BASE, mod.column, 1, mod.keycode);
                auto second = KeymapKey(BASE, second_column, 0, KC_V);
                set_keymap({first, second});

                EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(std::vector<uint8_t>{mod.modifier})));
                EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(std::vector<uint8_t>{mod.modifier, KC_V})));
                EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(std::vector<uint8_t>{mod.modifier})));
                EXPECT_EMPTY_REPORT(driver);
                first.press();
                run_one_scan_loop();
                idle_for(delay_ms);
                tap_key(second);
                first.release();
                run_one_scan_loop();
                VERIFY_AND_CLEAR(driver);
                idle_for(TAPPING_TERM + 1);
            }
        }
    }
}

TEST_F(EloraTapHold, every_distinct_modifier_pair_stacks_for_a_quick_shortcut) {
    TestDriver driver;
    std::vector<report_keyboard_t> reports;
    EXPECT_CALL(driver, send_keyboard_mock(_))
        .Times(testing::AnyNumber())
        .WillRepeatedly([&reports](report_keyboard_t& report) { reports.push_back(report); });

    for (const auto& first_mod : home_mods) {
        for (const auto& second_mod : home_mods) {
            if (first_mod.column == second_mod.column || first_mod.modifier == second_mod.modifier) {
                continue;
            }
            SCOPED_TRACE(std::string(get_keycode_string(first_mod.keycode)) + "+" + get_keycode_string(second_mod.keycode));
            auto first = KeymapKey(BASE, first_mod.column, 1, first_mod.keycode);
            auto second = KeymapKey(BASE, second_mod.column, 1, second_mod.keycode);
            auto letter = KeymapKey(BASE, 4, 0, KC_V);
            set_keymap({first, second, letter});
            reports.clear();

            first.press();
            run_one_scan_loop();
            second.press();
            run_one_scan_loop();
            tap_key(letter);
            second.release();
            run_one_scan_loop();
            first.release();
            run_one_scan_loop();
            idle_for(TAPPING_TERM + 1);

            const uint8_t required_mods = MOD_BIT(first_mod.modifier) | MOD_BIT(second_mod.modifier);
            const bool sent_chord = std::any_of(reports.begin(), reports.end(), [required_mods](const report_keyboard_t& report) {
                if ((report.mods & required_mods) != required_mods) {
                    return false;
                }
                return std::find(std::begin(report.keys), std::end(report.keys), KC_V) != std::end(report.keys);
            });
            EXPECT_TRUE(sent_chord);
            for (const auto& report : reports) {
                for (uint8_t code : report.keys) {
                    EXPECT_TRUE(code == KC_NO || code == KC_V);
                }
            }
        }
    }
}

TEST_F(EloraTapHold, every_modifier_can_tap_then_immediately_chord) {
    TestDriver driver;
    InSequence order;
    for (const auto& mod : home_mods) {
        SCOPED_TRACE(get_keycode_string(mod.keycode));
        auto first = KeymapKey(BASE, mod.column, 1, mod.keycode);
        auto letter = KeymapKey(BASE, mod.column < 5 ? 4 : 5, 0, KC_V);
        set_keymap({first, letter});

        EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(std::vector<uint8_t>{mod.tap})));
        EXPECT_EMPTY_REPORT(driver);
        tap_key(first);
        VERIFY_AND_CLEAR(driver);

        EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(std::vector<uint8_t>{mod.modifier})));
        EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(std::vector<uint8_t>{mod.modifier, KC_V})));
        EXPECT_CALL(driver, send_keyboard_mock(KeyboardReport(std::vector<uint8_t>{mod.modifier})));
        EXPECT_EMPTY_REPORT(driver);
        first.press();
        run_one_scan_loop();
        tap_key(letter);
        first.release();
        run_one_scan_loop();
        VERIFY_AND_CLEAR(driver);
        idle_for(TAPPING_TERM + 1);
    }
}

TEST_F(EloraTapHold, tapping_dedicated_shift_does_not_cancel_a_held_home_modifier) {
    TestDriver driver;
    std::vector<report_keyboard_t> reports;
    EXPECT_CALL(driver, send_keyboard_mock(_))
        .Times(testing::AnyNumber())
        .WillRepeatedly([&reports](report_keyboard_t& report) { reports.push_back(report); });

    for (const auto& mod : home_mods) {
        for (uint8_t shift_code : {KC_LEFT_SHIFT, KC_RIGHT_SHIFT}) {
            SCOPED_TRACE(std::string(get_keycode_string(mod.keycode)) + "+" + get_keycode_string(shift_code));
            auto first = KeymapKey(BASE, mod.column, 1, mod.keycode);
            auto shift = KeymapKey(BASE, shift_code == KC_LEFT_SHIFT ? 0 : 9, 2, shift_code);
            auto letter = KeymapKey(BASE, 4, 0, KC_V);
            set_keymap({first, shift, letter});
            reports.clear();

            first.press();
            run_one_scan_loop();
            tap_key(shift);
            tap_key(letter);
            first.release();
            run_one_scan_loop();
            idle_for(TAPPING_TERM + 1);

            const bool sent_modified_letter = std::any_of(reports.begin(), reports.end(), [&mod](const report_keyboard_t& report) {
                if ((report.mods & MOD_BIT(mod.modifier)) == 0) {
                    return false;
                }
                return std::find(std::begin(report.keys), std::end(report.keys), KC_V) != std::end(report.keys);
            });
            EXPECT_TRUE(sent_modified_letter);
        }
    }
}

TEST_F(EloraTapHold, releasing_home_shift_keeps_a_held_dedicated_shift) {
    TestDriver driver;
    std::vector<report_keyboard_t> reports;
    EXPECT_CALL(driver, send_keyboard_mock(_))
        .Times(testing::AnyNumber())
        .WillRepeatedly([&reports](report_keyboard_t& report) { reports.push_back(report); });

    for (const auto& mod : {home_mods[4], home_mods[5]}) {
        SCOPED_TRACE(get_keycode_string(mod.keycode));
        auto outer = KeymapKey(BASE, mod.column < 5 ? 0 : 9, 2, mod.modifier);
        auto home = KeymapKey(BASE, mod.column, 1, mod.keycode);
        auto letter = KeymapKey(BASE, 4, 0, KC_V);
        set_keymap({outer, home, letter});
        reports.clear();

        outer.press();
        run_one_scan_loop();
        home.press();
        idle_for(TAPPING_TERM + 1);
        home.release();
        run_one_scan_loop();
        tap_key(letter);
        outer.release();
        run_one_scan_loop();
        idle_for(TAPPING_TERM + 1);

        const bool sent_shifted_letter = std::any_of(reports.begin(), reports.end(), [&mod](const report_keyboard_t& report) {
            if ((report.mods & MOD_BIT(mod.modifier)) == 0) {
                return false;
            }
            return std::find(std::begin(report.keys), std::end(report.keys), KC_V) != std::end(report.keys);
        });
        EXPECT_TRUE(sent_shifted_letter);
        EXPECT_TRUE(reports.back().mods == 0);
    }
}

TEST_F(EloraTapHold, either_held_ctrl_survives_releasing_the_other) {
    TestDriver driver;
    std::vector<report_keyboard_t> reports;
    EXPECT_CALL(driver, send_keyboard_mock(_))
        .Times(testing::AnyNumber())
        .WillRepeatedly([&reports](report_keyboard_t& report) { reports.push_back(report); });

    for (const auto& pair : {std::pair<HomeModSpec, HomeModSpec>{home_mods[0], home_mods[3]},
                             std::pair<HomeModSpec, HomeModSpec>{home_mods[6], home_mods[9]}}) {
        for (bool reverse : {false, true}) {
            const auto& first_mod = reverse ? pair.second : pair.first;
            const auto& second_mod = reverse ? pair.first : pair.second;
            SCOPED_TRACE(std::string(get_keycode_string(first_mod.keycode)) + "+" + get_keycode_string(second_mod.keycode));
            auto first = KeymapKey(BASE, first_mod.column, 1, first_mod.keycode);
            auto second = KeymapKey(BASE, second_mod.column, 1, second_mod.keycode);
            auto letter = KeymapKey(BASE, 4, 0, KC_V);
            set_keymap({first, second, letter});
            reports.clear();

            first.press();
            idle_for(TAPPING_TERM + 1);
            second.press();
            idle_for(TAPPING_TERM + 1);
            first.release();
            run_one_scan_loop();
            tap_key(letter);
            second.release();
            run_one_scan_loop();
            idle_for(TAPPING_TERM + 1);

            const bool sent_ctrl_letter = std::any_of(reports.begin(), reports.end(), [&second_mod](const report_keyboard_t& report) {
                if ((report.mods & MOD_BIT(second_mod.modifier)) == 0) {
                    return false;
                }
                return std::find(std::begin(report.keys), std::end(report.keys), KC_V) != std::end(report.keys);
            });
            EXPECT_TRUE(sent_ctrl_letter);
            EXPECT_TRUE(reports.back().mods == 0);
        }
    }
}

TEST_F(EloraTapHold, plain_ctrl_and_home_ctrl_survive_either_release_order) {
    TestDriver driver;
    std::vector<report_keyboard_t> reports;
    EXPECT_CALL(driver, send_keyboard_mock(_))
        .Times(testing::AnyNumber())
        .WillRepeatedly([&reports](report_keyboard_t& report) { reports.push_back(report); });

    for (const auto& mod : {home_mods[0], home_mods[3], home_mods[6], home_mods[9]}) {
        for (bool release_home_first : {false, true}) {
            SCOPED_TRACE(std::string(get_keycode_string(mod.keycode)) + (release_home_first ? " home first" : " plain first"));
            auto home = KeymapKey(BASE, mod.column, 1, mod.keycode);
            auto plain = KeymapKey(BASE, mod.column < 5 ? 2 : 7, 0, mod.modifier);
            auto letter = KeymapKey(BASE, 4, 0, KC_V);
            set_keymap({home, plain, letter});
            reports.clear();

            home.press();
            idle_for(TAPPING_TERM + 1);
            plain.press();
            run_one_scan_loop();
            if (release_home_first) {
                home.release();
            } else {
                plain.release();
            }
            run_one_scan_loop();
            tap_key(letter);
            if (release_home_first) {
                plain.release();
            } else {
                home.release();
            }
            run_one_scan_loop();
            idle_for(TAPPING_TERM + 1);

            const bool sent_ctrl_letter = std::any_of(reports.begin(), reports.end(), [&mod](const report_keyboard_t& report) {
                if ((report.mods & MOD_BIT(mod.modifier)) == 0) {
                    return false;
                }
                return std::find(std::begin(report.keys), std::end(report.keys), KC_V) != std::end(report.keys);
            });
            EXPECT_TRUE(sent_ctrl_letter);
            EXPECT_TRUE(reports.back().mods == 0);
        }
    }
}

TEST_F(EloraTapHold, two_plain_shift_keys_survive_either_release_order) {
    TestDriver driver;
    std::vector<report_keyboard_t> reports;
    EXPECT_CALL(driver, send_keyboard_mock(_))
        .Times(testing::AnyNumber())
        .WillRepeatedly([&reports](report_keyboard_t& report) { reports.push_back(report); });

    for (bool right_side : {false, true}) {
        for (bool release_first : {false, true}) {
            uint8_t shift_code = right_side ? KC_RIGHT_SHIFT : KC_LEFT_SHIFT;
            SCOPED_TRACE(std::string(get_keycode_string(shift_code)) + (release_first ? " first key released" : " second key released"));
            auto first = KeymapKey(BASE, right_side ? 5 : 0, 2, shift_code);
            auto second = KeymapKey(BASE, right_side ? 9 : 4, 0, shift_code);
            auto letter = KeymapKey(BASE, 3, 0, KC_V);
            set_keymap({first, second, letter});
            reports.clear();

            first.press();
            run_one_scan_loop();
            second.press();
            run_one_scan_loop();
            if (release_first) {
                first.release();
            } else {
                second.release();
            }
            run_one_scan_loop();
            tap_key(letter);
            if (release_first) {
                second.release();
            } else {
                first.release();
            }
            run_one_scan_loop();
            idle_for(TAPPING_TERM + 1);

            const bool sent_shifted_letter = std::any_of(reports.begin(), reports.end(), [shift_code](const report_keyboard_t& report) {
                if ((report.mods & MOD_BIT(shift_code)) == 0) {
                    return false;
                }
                return std::find(std::begin(report.keys), std::end(report.keys), KC_V) != std::end(report.keys);
            });
            EXPECT_TRUE(sent_shifted_letter);
            EXPECT_TRUE(reports.back().mods == 0);
        }
    }
}

// USB suspend clears QMK's modifier bits without sending any release, which
// used to leave tap_hold.c withholding the dedicated key's release forever.
TEST_F(EloraTapHold, cleared_mods_do_not_stick_a_dedicated_modifier) {
    TestDriver driver;
    std::vector<report_keyboard_t> reports;
    EXPECT_CALL(driver, send_keyboard_mock(_))
        .Times(testing::AnyNumber())
        .WillRepeatedly([&reports](report_keyboard_t& report) { reports.push_back(report); });

    for (const auto& mod : {home_mods[0], home_mods[3], home_mods[4],
                            home_mods[5], home_mods[6], home_mods[9]}) {
        SCOPED_TRACE(get_keycode_string(mod.keycode));
        auto home = KeymapKey(BASE, mod.column, 1, mod.keycode);
        auto plain = KeymapKey(BASE, mod.column < 5 ? 0 : 9, 2, mod.modifier);
        auto letter = KeymapKey(BASE, 4, 0, KC_V);
        set_keymap({home, plain, letter});
        reports.clear();

        // Drop the bits the way a suspend does: key still down, no release.
        home.press();
        idle_for(TAPPING_TERM + 1);
        clear_keyboard();
        run_one_scan_loop();

        plain.press();
        run_one_scan_loop();
        tap_key(letter);
        plain.release();
        run_one_scan_loop();

        const bool sent_modified_letter = std::any_of(reports.begin(), reports.end(), [&mod](const report_keyboard_t& report) {
            if ((report.mods & MOD_BIT(mod.modifier)) == 0) {
                return false;
            }
            return std::find(std::begin(report.keys), std::end(report.keys), KC_V) != std::end(report.keys);
        });
        EXPECT_TRUE(sent_modified_letter);
        EXPECT_EQ(reports.back().mods, 0);

        home.release();
        idle_for(TAPPING_TERM + 1);
    }
}
