#!/usr/bin/env python3
"""Set the SplitKB TFT to portrait and keep its idle animation visible."""

import argparse
import re
from pathlib import Path


SETTINGS = {
    "LCD_WIDTH": ("240", "135"),
    "LCD_HEIGHT": ("135", "240"),
    "LCD_ROTATION": ("QP_ROTATION_90", "QP_ROTATION_0"),
    "LCD_OFFSET_X": ("40", "52"),
    "LCD_OFFSET_Y": ("53", "40"),
}


def patch_config(path: Path) -> None:
    source = path.read_text()
    for name, (landscape, portrait) in SETTINGS.items():
        pattern = re.compile(rf"^(\s*#\s*define\s+{name}\s+)(\S+)([^\n]*)$", re.MULTILINE)
        matches = list(pattern.finditer(source))
        if len(matches) != 1:
            raise SystemExit(f"Expected exactly one {name} definition in {path}; found {len(matches)}")
        match = matches[0]
        current = match.group(2)
        if current not in (landscape, portrait):
            raise SystemExit(f"Unexpected {name}={current} in {path}; inspect upstream display configuration")
        source = source[: match.start(2)] + portrait + source[match.end(2) :]

    # SplitKB's surface framebuffer currently uses literal dimensions rather
    # than the LCD macros. An upstream size change must be reviewed together
    # with the orientation patch to avoid writing beyond its buffer.
    display_source = path.with_name("hlc_tft_display.c").read_text()
    buffer_match = re.search(
        r"SURFACE_REQUIRED_BUFFER_BYTE_SIZE\(\s*(\d+)\s*,\s*(\d+)\s*,\s*16\s*\)",
        display_source,
    )
    if buffer_match and buffer_match.groups() != ("135", "240"):
        raise SystemExit("Unexpected TFT framebuffer size; inspect upstream display implementation")
    if not buffer_match and not re.search(
        r"SURFACE_REQUIRED_BUFFER_BYTE_SIZE\(\s*LCD_WIDTH\s*,\s*LCD_HEIGHT\s*,\s*16\s*\)",
        display_source,
    ):
        raise SystemExit("Cannot verify TFT framebuffer size; inspect upstream display implementation")

    # Both the backlight and Quantum Painter use this shared timeout. Drawing
    # animation frames does not count as input activity, so disable idle sleep.
    halcyon_config = path.parent.parent / "config.h"
    halcyon_source = halcyon_config.read_text()
    timeout = re.compile(
        r"^([ \t]*#[ \t]*define[ \t]+HLC_BACKLIGHT_TIMEOUT[ \t]+)(120000|0)([ \t]*(?://[^\n]*)?)$",
        re.MULTILINE,
    )
    halcyon_source, count = timeout.subn(r"\g<1>0\3", halcyon_source)
    if count != 1:
        raise SystemExit("Unexpected Halcyon backlight timeout; inspect upstream configuration")

    if source != path.read_text():
        path.write_text(source)
    if halcyon_source != halcyon_config.read_text():
        halcyon_config.write_text(halcyon_source)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("config", type=Path)
    patch_config(parser.parse_args().config)
