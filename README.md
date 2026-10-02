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

The supplied [project specification](docs/agreed-spec.md) is the source for this setup. The approved [sprite sheet](assets/tft_idle_smiley_sprite_sheet.png) is included separately for regenerating the display data.

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

### Layers

- **Base:** QWERTY and direct `1`–`0` number row.
- **Nav:** arrows on H/J/K/L, Home/End and Page Up/Page Down on the right upper row; left home row has Command/Option/Control/Shift.
- **Symbol:** common shifted punctuation on the upper alpha row and brackets/operators on the home row.
- **Number:** right-hand numpad-style 7–9, 4–6, 1–3, and 0.
- **Function:** F1–F12 on the alpha rows.
- **Media:** previous, play/pause, next, mute, and volume.
- **Mouse:** pointer directions and three mouse buttons. This is a key-controlled mouse layer; the unused Cirque module is not enabled.

Hold the corresponding thumb key to use each layer. The tapping term is 200 ms with chordal hold enabled. The right encoder turns **clockwise for Tab** and **counter-clockwise for Shift+Tab**. Its press can be assigned later in `right_halcyon_buttons` in [keymap.c](keyboards/splitkb/halcyon/elora/keymaps/elora/keymap.c).

The TFT is blank during ordinary typing. After about three seconds of no input, the yellow, pink and teal smiley jumps while its shadow stays on the ground. The laughing smile holds for four steps before landing. Activity clears the animation. A non-base layer name is shown while that layer is active. A lost split connection shows `SPLIT` in red after a short startup grace period, without hiding the idle animation. There is no Caps Lock display indicator. Host and module fault detection are future work.

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

   The script clones SplitKB's userspace into `.build/` on first use, copies this keymap there, applies the portrait TFT orientation, and builds each module variant. The expected output is:

   ```text
   .build/firmware/elora_left_tft.uf2
   .build/firmware/elora_right_encoder_rev2.uf2
   ```

   Set `SPLITKB_USERSPACE=/path/to/your/halcyon-qmk/checkout` to use an existing checkout. The script changes that checkout's Elora keymap, `qmk.json`, and TFT configuration, so use a disposable checkout. The build also sets QMK's `user.overlay_dir` to that checkout.

The build source is [keymap.c](keyboards/splitkb/halcyon/elora/keymaps/elora/keymap.c), [config.h](keyboards/splitkb/halcyon/elora/keymaps/elora/config.h), [rules.mk](keyboards/splitkb/halcyon/elora/keymaps/elora/rules.mk), and [display.c](keyboards/splitkb/halcyon/elora/keymaps/elora/display.c). To regenerate the committed smiley data and preview GIF after editing the PNG, install Pillow and run `python3 scripts/generate_smiley.py`.

The supplied UF2 images were compiled with SplitKB userspace commit `0e44cb2c1c26db8a63d0331ebad85bd8df75bd18`, QMK firmware commit `7a1bbf37c5c07da4ea0a162bb139083a46ef40a1`, and `arm-none-eabi-gcc` 8.5.0. The build script uses the current `halcyon-qmk` branch unless `SPLITKB_USERSPACE` points to a specific checkout. Each successful build refreshes both firmware images and checksums in `firmware/`.

## Flash the right half

The right firmware is ready at [firmware/elora_right_encoder_rev2.uf2](firmware/elora_right_encoder_rev2.uf2). This is a different image from the left TFT firmware.

1. Connect the **right half** by USB-C. Double-tap its controller reset button to mount `RPI-RP2`.
2. Run `./scripts/flash-right.sh`, or drag [firmware/elora_right_encoder_rev2.uf2](firmware/elora_right_encoder_rev2.uf2) onto `RPI-RP2` in Finder.
3. Wait for the drive to disappear, then reconnect both halves. Test the keys and encoder: clockwise sends Tab; counterclockwise sends Shift+Tab. The encoder's push switch is supported by the module, but this layout leaves it unassigned.

The flash script checks the UF2 checksum and stops if the bootloader drive is absent. Confirm that the mounted controller is the **right** one before copying. See [SplitKB's wired firmware guide](https://docs.splitkb.com/product-guides/halcyon-series/build-guide/wired/firmware) and [encoder installation guide](https://docs.splitkb.com/product-guides/halcyon-series/build-guide/wired/modules/encoder). If the press does not move, raise the knob a couple of millimetres above the module as SplitKB instructs.

## Current tuning points

- Home-row hold timing can be changed in [config.h](keyboards/splitkb/halcyon/elora/keymaps/elora/config.h).
- Layer assignments and encoder direction can be changed in [keymap.c](keyboards/splitkb/halcyon/elora/keymaps/elora/keymap.c).
- The idle delay, frame speed and layer notice can be changed in [display.c](keyboards/splitkb/halcyon/elora/keymaps/elora/display.c).
- The `halcyon-qmk` branch can change over time; if a future upstream update changes its module or `LAYOUT(...)` API, inspect the new definitions before updating this keymap.
