# Project guidance

## Project

- This repository builds QMK firmware for a wired SplitKB Halcyon Elora rev2 (Choc).
- The left RP2040 controller has a 135 × 240 portrait ST7789 TFT. The right controller has a Halcyon Encoder Rev. 2. They require different UF2 images.
- `README.md` describes the current layout, display behavior, tool setup, and flashing steps. `docs/agreed-spec.md` records the original agreement; later changes are in the README and firmware source.

## Source of truth

- Edit the keymap in `src/`. Do not edit the copy under `.build/splitkb-qmk-userspace/`; `scripts/build.sh` overwrites it.
- `keymap.c` defines layers and encoder behavior. `tap_hold.c` and `config.h` define tap and hold behavior. `keylog.c` collects recent keys and synchronizes them to the TFT. `display.c` implements TFT status, recent keys from each half, active modifiers, and the idle animation.
- `scripts/patch_display.py` applies the portrait TFT settings and disables idle display shutdown in SplitKB's userspace during the build. `scripts/generate_smiley.py` regenerates `smiley.c`, `smiley.h`, and the preview GIF from `assets/tft_idle_smiley_sprite_sheet.png`.
- The left TFT build and right encoder build are selected in `scripts/build.sh` and `qmk.json`. Keep both variants working when changing shared code.

## Build and verification

- Follow the toolchain setup in `README.md`, then run `./scripts/build.sh`. A successful build updates both `firmware/*.uf2` images and their `.sha256` files.
- For key behavior changes, run `python3 scripts/simulate_keymap.py`. With a configured QMK checkout, run `./scripts/run_qmk_tap_tests.sh` for native tap and hold coverage.
- Check `git diff --check` and verify both firmware checksums after rebuilding. Physical keyboard behavior still needs testing after flashing.
- `scripts/flash-left.sh` and `scripts/flash-right.sh` copy different images to an `RPI-RP2` bootloader drive. Confirm which half is mounted before flashing.

## Working conventions

- Preserve existing user changes; this workspace may have uncommitted work.
- Address and remove inline `FFF:` correction comments. If one is ambiguous or contradicts a prior goal, ask for clarification. If it asks a question, answer it and check whether the comment can be removed.
- Prefer concise bulleted or numbered updates over large tables.

## Design and implementation guidelines

* Be pragmatic in decision making.
* Code must be human readable and comprehensible.
* Use explicit, descriptive variable and constant names that include units for measured values, such as `idle_elapsed_ms` and `SYNC_INTERVAL_MS`.
* Add a concise one-line comment explaining each configuration constant, including its units and any special value such as zero disabling a feature. Preserve names required by QMK or another external API, and explain their meaning and units in the comment.
* The project is for life-time usage with evolution and maintenance goals.
* The design and implementation must be well-reasoned.
* Use best C and general software development practices when developing.
* Before implementing to a design that verifying decisions in it.
* After implementing to a complete review of the change.
* Do not guess, if something ambiguous, ask question.
