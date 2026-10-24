#!/usr/bin/env bash
set -euo pipefail

root="$(cd "$(dirname "$0")/.." && pwd)"
vendor="${SPLITKB_USERSPACE:-$root/.build/splitkb-qmk-userspace}"
mkdir -p "$(dirname "$vendor")"
if [[ ! -d "$vendor/.git" ]]; then
  git clone --depth 1 --branch halcyon-qmk https://github.com/splitkb/qmk_userspace.git "$vendor"
fi

# Replace outright, or a source dropped from src/ stays behind and compiles.
keymap_dest="$vendor/keyboards/splitkb/halcyon/elora/keymaps/elora"
rm -rf "$keymap_dest"
mkdir -p "$keymap_dest"
cp "$root"/src/* "$keymap_dest/"
cp "$root/qmk.json" "$vendor/qmk.json"
python3 "$root/scripts/patch_display.py" "$vendor/users/halcyon_modules/splitkb/hlc_tft_display/config.h"

qmk config user.overlay_dir="$vendor" >/dev/null
cd "$vendor"

qmk compile -kb splitkb/halcyon/elora/rev2 -km elora -e HLC_TFT_DISPLAY=1 -e TARGET=elora_left_tft
qmk compile -kb splitkb/halcyon/elora/rev2 -km elora -e HLC_ENCODER_REV2=1 -e TARGET=elora_right_encoder_rev2

# Publish only after both compiles, so a failure cannot half-update firmware/.
mkdir -p "$root/firmware"
cp "$vendor/elora_left_tft.uf2" "$vendor/elora_right_encoder_rev2.uf2" "$root/firmware/"
echo "Firmware in $root/firmware"
