#!/usr/bin/env python3
"""Turn the TFT output 90 degrees anticlockwise from the old landscape build."""
import sys
from pathlib import Path

path = Path(sys.argv[1])
source = path.read_text()
settings = {
    "LCD_WIDTH": ("240", "135"),
    "LCD_HEIGHT": ("135", "240"),
    "LCD_ROTATION": ("QP_ROTATION_90", "QP_ROTATION_0"),
    "LCD_OFFSET_X": ("40", "52"),
    "LCD_OFFSET_Y": ("53", "40"),
}
for name, (old, new) in settings.items():
    before = f"#define {name} {old}"
    after = f"#define {name} {new}"
    if before in source:
        source = source.replace(before, after, 1)
    elif after not in source:
        raise SystemExit(f"Unexpected {name} in {path}; inspect upstream display configuration")
path.write_text(source)
