# Elora Keyboard Project — Agreed Specification

_Last consolidated: 2 October 2026_

This is the current source of truth for the keyboard project.

## Hardware

- Keyboard: **SplitKB Halcyon Elora — Choc**
- Controllers: **Wired Halcyon Controller**, pair with Module Connection Kit
- MCU: **RP2040** / ARM Cortex-M0+
- Switches: **Ambients Silent Kailh Low Profile Choc — Twilight, linear, 35gf**
- Keycaps: **MBK Legend‡ 60s — Red on Black**
- Case: **Halcyon Elora Low Profile Case**
  - Tinted cover set
  - Clear acrylic bottom puck plates
  - Bead-blasted aluminium top plates
- **Aurora Low Profile Case Hardware Kit**
- **Tenting Puck — pair, Silver**
- USB-C host and interconnect cables

## Purchased modules

- **Halcyon Rotary Encoder Module — Silver, Revision 2**
- **Halcyon Cirque Touchpad Module**
- **Halcyon TFT display**

## Modules used

- **Left half:** TFT display
- **Right half:** Rotary Encoder Rev. 2
- **Cirque:** purchased but not used

## Firmware

Use **QMK**, based on SplitKB's current Halcyon `halcyon-qmk` userspace.

Build:
- Target: RP2040
- Compiler: `arm-none-eabi-gcc`
- Output: `.uf2`
- Left build: Elora + TFT
- Right build: Elora + Encoder Rev. 2

Initial configuration direction:

```c
#define TAPPING_TERM 200
#define CHORDAL_HOLD
#define SPLIT_ACTIVITY_ENABLE
#define SPLIT_LAYER_STATE_ENABLE
#define SPLIT_MODS_ENABLE
```

macOS semantics:
- GUI = Command
- Alt = Option
- Ctrl = Control

## Base logical layout

Standard **QWERTY** with a direct number row.

```text
1 2 3 4 5        6 7 8 9 0
Q W E R T        Y U I O P
A S D F G        H J K L ;
Z X C V B        N M , . /
```

## Home-row mods

Left:
- A = tap A / hold Command
- S = tap S / hold Option
- D = tap D / hold Control
- F = tap F / hold Shift

Right:
- J = tap J / hold Shift
- K = tap K / hold Control
- L = tap L / hold Option
- ; = tap ; / hold Command

## Outer / extra keys

Left outer column:
- Esc
- Tab
- Ctrl / Esc
- Shift

Right outer column:
- Esc
- Backspace
- Ctrl / '
- Shift

Extra arrows:
- Left side: **↑**, **↓**
- Right side: **→**, **←**

No Page Up / Page Down on BASE.

## Thumb keys

Left:
- Esc / Media
- Space / Nav
- Tab / Mouse

Right:
- Enter / Symbol
- Backspace / Number
- Delete / Function

Far outer thumb positions:
- **unassigned**
- reserved for future macros/functions

# Exact physical keycap mapping

## Visual mapping

Top row:

```text
Esc | 1  2  3  4  5        6  7  8  9  0 | Esc
```

Second row:

```text
Tab | Q  W  E  R  T        Y  U  I  O  P | Backspace
```

Home row:

```text
Blank | A  S  D  F*  G     H  J*  K  L  ; | Blank
```

`F*` and `J*` use the **homing keycaps**.

The blank outer home-row caps mean:
- left blank = **Ctrl / Esc**
- right blank = **Ctrl / '**

Bottom alpha row:

```text
Shift | Z  X  C  V  B      N  M  ,  .  / | Shift
```

Upper extra/thumb-adjacent positions:

```text
Blank | ↑                         → | Blank
```

Lowest thumb row, left-to-right:

```text
Blank
Convex blank (Esc / Media)
Convex Space (Space / Nav)
Convex Tab (Tab / Mouse)
↓
←
Convex Enter (Enter / Symbol)
Convex Backspace (Backspace / Number)
Blank (Delete / Function)
Blank
```

## Installation list — top-left to bottom-right

### Row 1
1. Esc
2. 1
3. 2
4. 3
5. 4
6. 5
7. 6
8. 7
9. 8
10. 9
11. 0
12. Esc

### Row 2
13. Tab
14. Q
15. W
16. E
17. R
18. T
19. Y
20. U
21. I
22. O
23. P
24. Backspace

### Row 3
25. Blank — Ctrl / Esc
26. A
27. S
28. D
29. **F homing**
30. G
31. H
32. **J homing**
33. K
34. L
35. ;
36. Blank — Ctrl / '

### Row 4
37. Shift
38. Z
39. X
40. C
41. V
42. B
43. N
44. M
45. ,
46. .
47. /
48. Shift

### Upper extra positions
49. Blank
50. ↑
51. →
52. Blank

### Lowest thumb row
53. Blank — far-left unused
54. **Convex blank** — Esc / Media
55. **Convex Space** — Space / Nav
56. **Convex Tab** — Tab / Mouse
57. ↓
58. ←
59. **Convex Enter** — Enter / Symbol
60. **Convex Backspace** — Backspace / Number
61. Blank — Delete / Function
62. Blank — far-right unused

## Keycap shape rules

Use **convex 1u caps** for:
- Esc / Media
- Space / Nav
- Tab / Mouse
- Enter / Symbol
- Backspace / Number

Use **flat blank caps** for:
- Ctrl / Esc
- Ctrl / '
- Delete / Function
- unused extra positions
- unused far-left / far-right thumb positions

Use **homing caps** for:
- F
- J

## Layers

- BASE
- NAV
- SYM
- NUM
- FUN
- MEDIA
- MOUSE

## Rotary encoder

Right rotary encoder:
- **Clockwise:** Tab
- **Counter-clockwise:** Shift+Tab
- **Press:** not fixed yet; leave configurable

Conceptual QMK behavior:

```c
bool encoder_update_user(uint8_t index, bool clockwise) {
    if (clockwise) {
        tap_code(KC_TAB);
    } else {
        tap_code16(S(KC_TAB));
    }
    return false;
}
```

## TFT display

- Halcyon TFT
- **1.14 inch**
- **240 × 135 px**
- Landscape
- ST7789
- Full colour

Behavior:
- **No Caps Lock indicator**
- Normal state stays visually quiet
- Show only relevant state/issues

Possible issue/status messages:
- non-base layer when useful during learning/debugging
- unexpectedly held/stuck modifier
- split-half communication failure
- module problem
- USB/host issue
- bootloader/reset state

## TFT idle animation

After approximately **3 seconds without keyboard or encoder activity**, start the idle animation.

Any keyboard or encoder activity stops it immediately.

Agreed style:
- yellow grungy smiley
- rough black outlines
- hot-pink accents
- cyan/teal accents
- no legs
- no stars
- bouncy / lightly dancing
- smiling / laughing

Target:
- **240 × 135**
- about **8 frames**

Animation concept:
1. neutral
2. squash
3. tilt left
4. rebound
5. laugh
6. tilt right
7. squash
8. neutral

The approved sprite-sheet reference is included in this ZIP as:

`assets/tft_idle_smiley_sprite_sheet.png`

## Source structure

Planned structure:

```text
keymap.c
config.h
rules.mk
display / idle logic
animation assets
```

Do not guess SplitKB's physical `LAYOUT(...)` ordering. Read it from the checked-out `halcyon-qmk` branch.

## macOS build workflow

Docker is optional. Start with native QMK.

```bash
curl -fsSL https://install.qmk.fm | sh
qmk setup

mkdir -p ~/src
cd ~/src
git clone --branch halcyon-qmk https://github.com/splitkb/qmk_userspace.git splitkb-qmk-userspace
cd splitkb-qmk-userspace
qmk config user.overlay_dir="$(pwd)"
qmk userspace-list
```

Inspect the current Elora hardware definition:

```bash
find keyboards/splitkb/halcyon/elora -type f | sort
grep -R "LAYOUT" keyboards/splitkb/halcyon/elora | head -50
```

Workflow:

```text
edit source
→ compile with QMK / arm-none-eabi-gcc
→ produce UF2
→ enter RP2040 bootloader
→ RPI-RP2 appears on macOS
→ copy UF2
→ controller reboots
```

## Repository plan

Repository:

`ChoppinBlockParty/elora-keyboard`

Planned branch:

`feature/custom-elora-qmk`

## Implementation order

Phase 1:
- exact physical keymap
- QWERTY + number row
- outer columns + arrows
- home-row mods
- thumb tap/hold keys
- layer skeleton
- encoder Tab / Shift+Tab
- TFT support
- 3-second idle animation
- basic issue rendering

Phase 2:
- tune tapping term / home-row mods
- refine NAV/SYM/NUM
- refine TFT warning rules

Phase 3:
- macros
- optional Windows/Citrix mode
- TFT polish
- optional RGB behavior
