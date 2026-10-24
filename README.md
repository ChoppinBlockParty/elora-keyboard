# Elora keyboard firmware

QMK firmware for a wired **SplitKB Halcyon Elora rev2 (Choc)** with a TFT display on the left and a **Halcyon Encoder Rev. 2** on the right. The two RP2040 controllers need different UF2 images.

## Flash the left half now

The left firmware is already built at [firmware/elora_left_tft.uf2](firmware/elora_left_tft.uf2). You do not need QMK or a compiler to install it.

1. Connect the **left half** to your Mac with USB-C.
2. Double-tap the small reset button on its wired Halcyon controller. A drive named `RPI-RP2` should appear. If it does not, hold the controller's boot button while reconnecting USB; see [SplitKB's bootloader instructions](https://docs.splitkb.com/product-guides/halcyon-series/using/using-wired).
3. From this repository, run `./scripts/flash-left.sh`. The script checks the UF2 checksum and copies it to the mounted controller. Alternatively, drag [firmware/elora_left_tft.uf2](firmware/elora_left_tft.uf2) onto `RPI-RP2` in Finder.
4. The drive should disappear as the controller restarts. With only the left half connected, the TFT may show `SPLIT` until the right half is connected; the idle smiley still animates after three seconds.

The script will stop without writing if `RPI-RP2` is not mounted. Ensure that the mounted controller is the **left** one; the right half needs its own firmware.

## Hardware

- Elora Choc PCB, Halcyon wired controllers and Module Connection Kit
- Ambients Silent Kailh Choc Twilight linear switches, 35 gf
- MBK Legend‡ 60s Red on Black keycaps, with F/J homing caps
- Halcyon Elora low profile case, tinted covers, clear acrylic bottom puck plates, bead blasted aluminium top plates, Aurora Low Profile Case Hardware Kit, and silver tenting pucks
- USB-C host and interconnect cables
- Left: Halcyon 1.14-inch ST7789 TFT, with 135 × 240 portrait output after a 90° anticlockwise turn from the first firmware build
- Right: silver Halcyon Rotary Encoder Rev. 2
- Cirque touchpad was purchased but is not installed

The [project specification](docs/agreed-spec.md) records the original hardware and layout decisions. Later changes, including the portrait TFT orientation and tap-and-hold behavior, are documented here and implemented in the checked-in source. The approved [sprite sheet](assets/tft_idle_smiley_sprite_sheet.png) is included for regenerating the display data.

## Physical key layout

The code uses the 62-position `LAYOUT(...)` order in [SplitKB's Elora default keymap](https://github.com/splitkb/qmk_userspace/blob/halcyon-qmk/keyboards/splitkb/halcyon/elora/keymaps/default_hlc/keymap.c). Each half has 6 keys on each of the first three rows, 8 on row four, and 5 thumb keys.

```text
Esc    1 2 3 4 5                       6 7 8 9 0    Esc
Tab    Q W E R T                       Y U I O P    Backspace
Ctrl/Esc A S D F G                     H J K L ;    Ctrl/'
Shift  Z X C V B  Blank ↑          → Blank N M , . / Shift
             Blank Esc/Media Space/Nav Tab/Mouse ↓  ← Enter/Sym Backspace/Num Delete/Fun Blank
```

- A/S/D/F hold Command/Option/Control/Shift; J/K/L/; hold Shift/Control/Option/Command. The tap is the printed character.
- The blank outer home keys are Ctrl/Esc and Ctrl/'. The outer bottom keys are Shift.
- The upper extra positions are blank, Up, Right, blank. The lower inner positions are Down and Left. There are no Page Up or Page Down keys on Base.
- The far left and far right thumb positions are unassigned. The encoder press is also unassigned. Blank caps are used for those positions and the Ctrl and Delete dual keys. The five main dual-function thumb keys use convex caps.
- On macOS, QMK GUI is Command, Alt is Option, and Ctrl is Control.

This is a custom QWERTY layout inspired by [Miryoku](https://github.com/manna-harbour/miryoku), not an official Miryoku keymap. Its tap and hold behavior is implemented with [QMK's documented options](https://docs.qmk.fm/tap_hold) in `tap_hold.c`.

### Layers

- **Base:** QWERTY and direct `1`–`0` number row.
- **Nav:** arrows on the right H/J/K/L positions, Home/End and Page Up/Page Down on the right upper row; left home row has Command/Option/Control/Shift.
- **Symbol:** punctuation on the left alpha rows and left number row, reached with the right Enter/Sym thumb.
- **Number:** 7–9, 4–6, and 1–3 on the left W/E/R, S/D/F, and X/C/V positions; 0 on G and decimal point on B, reached with the right Backspace/Num thumb.
- **Function:** F1–F12 on the left alpha rows, reached with the right Delete/Fun thumb.
- **Media:** previous, volume down/up, and next on the right home row; play/pause and mute on the right bottom row, reached with the left Esc/Media thumb.
- **Mouse:** pointer directions and three mouse buttons. This is a key-controlled mouse layer; the unused Cirque module is not enabled.

Hold the corresponding thumb key to use each layer. The tapping term is 200 ms. Opposite-hand layer chords activate immediately; a same-hand roll within 200 ms remains a thumb tap. Quick-tap repeat is disabled so tapping a thumb and immediately holding it still selects its layer. Every base home-row modifier, plus Ctrl/Esc and Ctrl/quote, uses QMK Permissive Hold for every second key on either hand. Press the modifier key, tap and release the second key, then release the modifier key to send a shortcut, even when the sequence is fast: J+semicolon produces `:`, and F+V produces `V`. For an ordinary `j;` or `fv` roll, release J or F before releasing the next key. If the modifier key has already been held for 200 ms, it acts as a modifier regardless of release order. The firmware also keeps Shift or Ctrl active when two keys supplying the same modifier overlap and one is released. These event sequences are inherently ambiguous until a key releases, so no firmware can infer a different intent from identical press and release events. The right encoder turns **clockwise for Tab** and **counter-clockwise for Shift+Tab**. Its press can be assigned later in `right_halcyon_buttons` in [keymap.c](src/keymap.c).

During typing, the TFT shows the most recently pressed key from each half as `L:` and `R:`, with USB connected to either half. Both halves must run the updated firmware for split key-label synchronization. Below them it lists active Ctrl, Shift, Alt and GUI modifiers; GUI is labelled `CMD/WIN` because it is the Command key on macOS and the Windows key on Windows. The key display stays visible while a modifier is active. After about three seconds of no input and with no active modifier, the yellow, pink and teal smiley jumps while its shadow stays on the ground. The laughing smile holds for four steps before landing. The display and backlight remain on during inactivity, so the animation continues beyond two minutes. Activity clears the animation and returns to the key display. A non-base layer name is shown while that layer is active. A lost split connection shows `SPLIT` in red after a short startup grace period, without hiding the idle animation. There is no Caps Lock display indicator. Host and module fault detection are future work.

After flashing, check the timing on the keyboard itself:

- Hold J, tap semicolon several times, then release J: expect only `:` characters.
- Roll J into semicolon by releasing J first: expect `j;`.
- Repeat with F and V: a nested tap gives `V`; a roll gives `fv`.
- Hold F and tap the dedicated left Shift, then tap V while still holding F: expect `V`.
- Connect USB to each half in turn and type on both halves: expect both `L:` and `R:` labels to update.
- Leave the keyboard idle for more than two minutes: expect the TFT to stay lit and the smiley to keep jumping.

![Preview of the TFT idle smiley jumping and holding its smile, enlarged three times](assets/tft_idle_preview.gif)

The GIF shows the 135 × 240 portrait output at 3× size. The firmware starts this 11-step loop after the three-second idle delay and plays each step for 212 ms (85% of the previous speed). Four steps hold the laughing smile. The white sprite background is transparent; a thin pale edge keeps its black outlines visible on the TFT's black background.

## Install the build tools on macOS

The build uses [SplitKB's `halcyon-qmk` userspace](https://github.com/splitkb/qmk_userspace/tree/halcyon-qmk) and [upstream QMK](https://github.com/qmk/qmk_firmware). Docker is optional.

1. Install the QMK CLI and the ARM toolchain. These commands use a project-local Python environment and the QMK-compatible osx-cross compiler:

   ```sh
   python3 -m venv .build/qmk-venv
   .build/qmk-venv/bin/python -m pip install qmk
   brew tap osx-cross/arm
   brew install osx-cross/arm/arm-none-eabi-gcc@8
   export PATH="$(pwd)/.build/qmk-venv/bin:$(brew --prefix arm-none-eabi-gcc@8)/bin:$(brew --prefix arm-none-eabi-binutils)/bin:$PATH"
   ```

   If Homebrew asks to trust the osx-cross binutils formula, run `brew trust --formula osx-cross/arm/arm-none-eabi-binutils` and repeat the install. Follow the [QMK setup guide](https://docs.qmk.fm/newbs_getting_started) for other tool installation options.

2. Set up an upstream QMK checkout:

   ```sh
   git clone --recursive https://github.com/qmk/qmk_firmware.git ~/src/qmk_firmware
   qmk setup -H ~/src/qmk_firmware
   ```

3. Run the build from this repository:

   ```sh
   ./scripts/build.sh
   ```

   The script clones SplitKB's userspace into `.build/` on first use, copies this keymap there, verifies the TFT framebuffer before applying the portrait orientation, and builds both module variants before refreshing either firmware image. The expected output is:

   ```text
   .build/firmware/elora_left_tft.uf2
   .build/firmware/elora_right_encoder_rev2.uf2
   ```

   Set `SPLITKB_USERSPACE=/path/to/your/halcyon-qmk/checkout` to use an existing checkout. The script changes that checkout's Elora keymap, `qmk.json`, TFT configuration, and shared backlight timeout, so use a disposable checkout. The build also sets QMK's `user.overlay_dir` to that checkout.

The build source is in `src/`: [keymap.c](src/keymap.c), [tap_hold.c](src/tap_hold.c), [config.h](src/config.h), [rules.mk](src/rules.mk), [keylog.c](src/keylog.c), and [display.c](src/display.c). To regenerate the committed smiley data and preview GIF after editing the PNG, install Pillow and run `python3 scripts/generate_smiley.py`.

Run `python3 scripts/simulate_keymap.py` to check representative layer chords, same-hand rolls, timeout behavior, and transparent key fallthrough against the checked-in keymap. It models QMK tap/hold rules; the actual keyboard still needs physical testing.

To run the native QMK tap/hold and key-label tests against the firmware's `tap_hold.c`, set `QMK_HOME` to your QMK checkout and run `./scripts/run_qmk_tap_tests.sh`. The checkout needs QMK's `lib/googletest` submodule (`git submodule update --init lib/googletest`). These tests cover real QMK press/release behavior for every home-row modifier with letter, number, punctuation, arrow, thumb layer-tap, and other modifier-tap keys, plus rolls, tapping-term boundaries, held layers, stacked modifiers, and overlapping Shift or Ctrl sources. The key-label test uses a simulated split transport to check USB roles, label delivery, throttled retries, and recovery after a display restart.

For clangd, generate the ignored local compilation database after a left firmware build:

```sh
python3 scripts/generate_compile_commands.py --qmk-home ~/src/qmk_firmware
```

Use your actual QMK checkout path (`.build/qmk_firmware` is the default). The generator uses the ARM compiler on `PATH` to discover system headers; pass `--arm-gcc /path/to/arm-none-eabi-gcc` if needed. It sets the ARM target and Cortex-M0+ CPU for all project C files.

The supplied UF2 images were compiled with SplitKB userspace commit `0e44cb2c1c26db8a63d0331ebad85bd8df75bd18`, QMK firmware commit `7a1bbf37c5c07da4ea0a162bb139083a46ef40a1`, and `arm-none-eabi-gcc` 8.5.0. The build script uses the current `halcyon-qmk` branch unless `SPLITKB_USERSPACE` points to a specific checkout. Each successful build refreshes both firmware images and checksums in `firmware/`.

## Flash the right half

The right firmware is ready at [firmware/elora_right_encoder_rev2.uf2](firmware/elora_right_encoder_rev2.uf2). This is a different image from the left TFT firmware.

1. Connect the **right half** by USB-C. Double-tap its controller reset button to mount `RPI-RP2`.
2. Run `./scripts/flash-right.sh`, or drag [firmware/elora_right_encoder_rev2.uf2](firmware/elora_right_encoder_rev2.uf2) onto `RPI-RP2` in Finder.
3. Wait for the drive to disappear, then reconnect both halves. Test the keys and encoder: clockwise sends Tab; counterclockwise sends Shift+Tab. The encoder's push switch is supported by the module, but this layout leaves it unassigned.

The flash script checks the UF2 checksum and stops if the bootloader drive is absent. Confirm that the mounted controller is the **right** one before copying. See [SplitKB's wired firmware guide](https://docs.splitkb.com/product-guides/halcyon-series/build-guide/wired/firmware) and [encoder installation guide](https://docs.splitkb.com/product-guides/halcyon-series/build-guide/wired/modules/encoder). If the press does not move, raise the knob a couple of millimetres above the module as SplitKB instructs.

## Current tuning points

- Home-row hold timing can be changed in [config.h](src/config.h).
- Layer assignments and encoder direction can be changed in [keymap.c](src/keymap.c).
- The idle delay, frame speed and layer notice can be changed in [display.c](src/display.c).
- The `halcyon-qmk` branch can change over time; if a future upstream update changes its module or `LAYOUT(...)` API, inspect the new definitions before updating this keymap.
