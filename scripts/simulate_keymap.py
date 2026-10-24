#!/usr/bin/env python3
"""Check key positions and representative QMK tap/hold chords without hardware.

This uses the LAYOUT arguments in keymap.c. Timing rules model the documented
Chordal Hold, per-key Permissive Hold, and Hold On Other Key Press behavior;
it is not QMK itself.
"""

import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
KEYMAP = ROOT / "src/keymap.c"
CONFIG = KEYMAP.with_name("config.h")
TAP_HOLD = KEYMAP.with_name("tap_hold.c")
source = KEYMAP.read_text()
tap_hold = TAP_HOLD.read_text()
config = CONFIG.read_text()


def arguments(start):
    depth = 1
    token = ""
    result = []
    for char in source[start:]:
        if char == "(":
            depth += 1
        elif char == ")":
            depth -= 1
            if depth == 0:
                result.append(token.strip())
                return result
        elif char == "," and depth == 1:
            result.append(token.strip())
            token = ""
            continue
        token += char
    raise ValueError("Unclosed LAYOUT call")


layers = {
    match.group(1): arguments(match.end())
    for match in re.finditer(r"\[(\w+)\]\s*=\s*LAYOUT\(", source)
}
assert set(layers) == {"BASE", "NAV", "SYM", "NUM", "FUN", "MEDIA", "MOUSE"}
assert all(len(keys) == 62 for keys in layers.values()), "Elora LAYOUT must have 62 keys"
assert "#define CHORDAL_HOLD" in config
quick_tap_disabled = "#define QUICK_TAP_TERM 0" in config
quick_layer_hold = (
    "#define HOLD_ON_OTHER_KEY_PRESS_PER_KEY" in config
    and "bool get_hold_on_other_key_press(" in tap_hold
    and all(
        f"case {name}:" in tap_hold
        for name in ("ESC_MED", "SPC_NAV", "TAB_MOU", "ENT_SYM", "BSP_NUM", "DEL_FUN")
    )
)
permissive_mod_taps = (
    "#define PERMISSIVE_HOLD_PER_KEY" in config
    and "bool get_permissive_hold(" in tap_hold
    and "return is_modifier_tap(keycode);" in tap_hold
)
home_mod_cases = ("GUI_A", "ALT_S", "CTL_D", "SFT_F", "SFT_J", "CTL_K", "ALT_L", "GUI_SCL", "CTL_ESC", "CTL_QUO")
assert all(f"case {name}:" in tap_hold for name in home_mod_cases)
all_modifier_chords = (
    "bool get_chordal_hold(" in tap_hold
    and "if (is_modifier_tap(tap_hold_keycode))" in tap_hold
)

# The first three rows have six keys per hand, the fourth eight per hand,
# and the thumb row five per hand. These are SplitKB's LAYOUT argument groups.
def hand(position):
    if position < 36:
        return "L" if position % 12 < 6 else "R"
    if position < 52:
        return "L" if position < 44 else "R"
    return "L" if position < 57 else "R"


thumbs = {"MEDIA": 53, "NAV": 54, "MOUSE": 55, "SYM": 58, "NUM": 59, "FUN": 60}

# All action keys should be on the hand opposite the activating thumb.
# Nav's left home-row modifiers are an intentional same-hand exception.
for layer, thumb in thumbs.items():
    for position, keycode in enumerate(layers[layer]):
        if keycode in ("___", "XXX"):
            continue
        if layer == "NAV" and position in (25, 26, 27, 28):
            continue
        assert hand(position) != hand(thumb), f"{layer} key {keycode} at {position} is on the thumb hand"


def layer_key(layer, position):
    value = layers[layer][position]
    return layers["BASE"][position] if value == "___" else value


def chord(layer, position, delay_ms=30):
    """Simulate pressing the layer thumb, then a key, then releasing both."""
    thumb = thumbs[layer]
    hold = delay_ms >= 200 or (quick_layer_hold and hand(thumb) != hand(position))
    if hold:
        return (layer_key(layer, position),)
    return (layers["BASE"][thumb], layers["BASE"][position])


def tap_then_hold(layer, delay_ms=50):
    """QMK's quick-tap term otherwise makes the second press repeat the tap."""
    return "LAYER" if quick_tap_disabled or delay_ms >= 200 else "REPEAT_TAP"


MODS = {
    "GUI_A": "GUI", "ALT_S": "ALT", "CTL_D": "CTRL", "SFT_F": "SHIFT",
    "SFT_J": "SHIFT", "CTL_K": "CTRL", "ALT_L": "ALT", "GUI_SCL": "GUI",
    "CTL_ESC": "CTRL", "CTL_QUO": "CTRL",
}


def mod_chord(mod_position, other_position, *, nested=True, delay_ms=30):
    """Model pressing a home mod, then another key, and their release order."""
    mod = layers["BASE"][mod_position]
    other = layers["BASE"][other_position]
    assert mod in MODS
    chord_allowed = hand(mod_position) != hand(other_position) or all_modifier_chords
    hold = delay_ms >= 200 or (chord_allowed and permissive_mod_taps and nested)
    return (MODS[mod], other) if hold else (mod, other)


cases = [
    ("Nav opposite-hand quick chord", chord("NAV", 31), ("KC_DOWN",)),
    ("Mouse opposite-hand quick chord", chord("MOUSE", 31), ("MS_LEFT",)),
    ("Media opposite-hand quick chord", chord("MEDIA", 31), ("KC_MPRV",)),
    ("Sym opposite-hand quick chord", chord("SYM", 14), ("KC_AT",)),
    ("Num opposite-hand quick chord", chord("NUM", 14), ("KC_7",)),
    ("Fun opposite-hand quick chord", chord("FUN", 13), ("KC_F1",)),
    ("Num same-hand roll stays a tap", chord("NUM", 31), ("BSP_NUM", "SFT_J")),
    ("Num deliberate hold after timeout", chord("NUM", 14, 210), ("KC_7",)),
    ("Nav transparent key falls through", chord("NAV", 13, 210), ("KC_Q",)),
    ("Tap then immediately hold Nav", tap_then_hold("NAV"), "LAYER"),
    ("Fast nested F+5 is Shift+5", mod_chord(28, 5), ("SHIFT", "KC_5")),
    ("Fast rolling F+5 stays a tap", mod_chord(28, 5, nested=False), ("SFT_F", "KC_5")),
    ("Fast nested F+V is Shift+V", mod_chord(28, 40), ("SHIFT", "KC_V")),
    ("Fast rolling F+V stays letters", mod_chord(28, 40, nested=False), ("SFT_F", "KC_V")),
    ("Held F+V shifts after timeout", mod_chord(28, 40, delay_ms=210), ("SHIFT", "KC_V")),
    ("Fast nested J+6 is Shift+6", mod_chord(31, 6), ("SHIFT", "KC_6")),
    ("Fast nested J+semicolon shifts", mod_chord(31, 34), ("SHIFT", "GUI_SCL")),
    ("Rolling J+semicolon stays taps", mod_chord(31, 34, nested=False), ("SFT_J", "GUI_SCL")),
    ("Fast nested A+1 is GUI+1", mod_chord(25, 1), ("GUI", "KC_1")),
    ("Fast nested S+2 is Alt+2", mod_chord(26, 2), ("ALT", "KC_2")),
    ("Fast nested D+3 is Ctrl+3", mod_chord(27, 3), ("CTRL", "KC_3")),
    ("Fast nested K+7 is Ctrl+7", mod_chord(32, 7), ("CTRL", "KC_7")),
    ("Fast nested L+8 is Alt+8", mod_chord(33, 8), ("ALT", "KC_8")),
    ("Fast nested semicolon+9 is GUI+9", mod_chord(34, 9), ("GUI", "KC_9")),
    ("Fast nested Ctrl/Esc+1 is Ctrl+1", mod_chord(24, 1), ("CTRL", "KC_1")),
    ("Fast nested Ctrl/quote+0 is Ctrl+0", mod_chord(35, 10), ("CTRL", "KC_0")),
    ("Fast nested F+Up is Shift+Up", mod_chord(28, 43), ("SHIFT", "KC_UP")),
    ("Cross-hand nested F+6 is Shift+6", mod_chord(28, 6), ("SHIFT", "KC_6")),
    ("Cross-hand rolling F+6 stays a tap", mod_chord(28, 6, nested=False), ("SFT_F", "KC_6")),
]

failures = 0
for name, actual, expected in cases:
    good = actual == expected
    print(f"{'PASS' if good else 'FAIL'} {name}: {actual}")
    failures += not good

if failures:
    raise SystemExit(f"{failures} scenario(s) failed")
print(f"All {len(cases)} scenarios passed")
