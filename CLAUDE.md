# Project guidance

## Project

- This repository builds QMK firmware for a wired SplitKB Halcyon Elora rev2 (Choc).
- The left RP2040 controller has a 135 × 240 portrait ST7789 TFT. The right controller has a Halcyon Encoder Rev. 2. They require different UF2 images.
- `README.md` describes the current layout, display behavior, tool setup, and flashing steps. `docs/agreed-spec.md` records the original agreement; later changes are in the README and firmware source.

## Source of truth

- Edit the keymap in `src/`. Do not edit the copy under `.build/splitkb-qmk-userspace/`; `scripts/build.sh` overwrites it.
- `keymap.c` defines layers and encoder behavior. `tap_hold.c` and `tap_hold_config.h` define tap and hold behavior; `tests/qmk_tap_hold/config.h` includes `tap_hold_config.h` so the two cannot drift. `config.h` adds the split-sync options the display needs. `keylog.c` collects recent keys and synchronizes them to the TFT. `display.c` implements TFT status, recent keys from each half, active modifiers, and the idle animation.
- `scripts/patch_display.py` applies the portrait TFT settings and disables idle display shutdown in SplitKB's userspace during the build. `scripts/generate_smiley.py` regenerates `smiley.c`, `smiley.h`, and the preview GIF from `assets/tft_idle_smiley_sprite_sheet.png`.
- `.clang-format` and `.clang-tidy` hold the style and check configuration. The keymap grid in `keymap.c` and the generated `smiley.c` sit inside `// clang-format off` guards; keep them there.
- The left TFT build and right encoder build are selected in `scripts/build.sh` and `qmk.json`. Keep both variants working when changing shared code.

## Build and verification

- Follow the toolchain setup in `README.md`, then run `./scripts/build.sh`. A successful build updates both `firmware/*.uf2` images. They are git-ignored build outputs, not committed artifacts.
- For key behavior changes, run `python3 scripts/simulate_keymap.py`. With a configured QMK checkout, run `./scripts/run_qmk_tap_tests.sh` for native tap and hold coverage.
- Run `./scripts/lint.sh` for clang-format and clang-tidy; `--fix` applies the formatting. `src/smiley.*` is generated and excluded.
- Check `git diff --check` and confirm both images were rebuilt. Physical keyboard behavior still needs testing after flashing.
- `scripts/flash-left.sh` and `scripts/flash-right.sh` copy different images to an `RPI-RP2` bootloader drive. Both halves expose an identical bootloader, so each script asks for confirmation unless given `--yes`.

## Working conventions

- Never run `git commit`, `git add`, `git stage`, or `git push`. Leave every change in the working tree; the user reviews and commits it themselves.
- Preserve existing user changes; this workspace may have uncommitted work.
- Address and remove inline `FFF:` correction comments. If one is ambiguous or contradicts a prior goal, ask for clarification. If it asks a question, answer it and check whether the comment can be removed.
- Prefer concise bulleted or numbered updates over large tables.

## Code comments

Comments are for the reader who already understands C and can see the code. Write one only when the code cannot carry the point itself.

- Default to no comment. Prefer a clearer name, a smaller function, or a named constant over a sentence explaining a muddled one.
- Never restate the code. A comment that names the thing below it (`// Left margin in pixels` on `TEXT_MARGIN_PX`) is noise, as is one that narrates control flow or labels a struct field after its own name.
- Comment the surprise: why a value is what it is, a constraint imposed from outside, a non-obvious consequence of an external API, or a decision whose alternative looks more sensible than it is. Say why, not what.
- Units and magic values belong in the name. Add a comment only when the name cannot hold the reason, such as an empirically chosen number or a value that upstream requires.
- Keep it to one or two lines, in plain prose, placed where it is needed rather than in a block at the top.
- A comment that would go stale the next time the code moves is already wrong. Prefer stating the invariant over describing the current sequence of steps.
- Delete a comment that no longer matches the code rather than leaving it to mislead.

## Design and implementation guidelines

* Be pragmatic in decision making.
* Code must be human readable and comprehensible.
* Use explicit, descriptive variable and constant names that include units for measured values, such as `idle_elapsed_ms` and `SYNC_INTERVAL_MS`.
* Preserve names required by QMK or another external API even when they read poorly, and carry units in names you do choose. See `## Code comments` for when to explain one.
* The project is for life-time usage with evolution and maintenance goals.
* The design and implementation must be well-reasoned.
* Use best C and general software development practices when developing.
* Before implementing to a design that verifying decisions in it.
* After implementing to a complete review of the change.
* Do not guess, if something ambiguous, ask question.
