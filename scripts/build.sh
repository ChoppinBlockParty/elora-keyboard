#!/usr/bin/env bash
set -euo pipefail

root="$(cd "$(dirname "$0")/.." && pwd)"
vendor="${SPLITKB_USERSPACE:-$root/.build/splitkb-qmk-userspace}"
mkdir -p "$(dirname "$vendor")" "$root/.build/firmware"
if [[ ! -d "$vendor/.git" ]]; then
  git clone --depth 1 --branch halcyon-qmk https://github.com/splitkb/qmk_userspace.git "$vendor"
fi

keymap_dest="$vendor/keyboards/splitkb/halcyon/elora/keymaps/elora"
mkdir -p "$keymap_dest"
cp "$root"/keyboards/splitkb/halcyon/elora/keymaps/elora/* "$keymap_dest/"
cp "$root/qmk.json" "$vendor/qmk.json"
python3 "$root/scripts/patch_display.py" "$vendor/users/halcyon_modules/splitkb/hlc_tft_display/config.h"

qmk config user.overlay_dir="$vendor" >/dev/null
cd "$vendor"

qmk compile -kb splitkb/halcyon/elora/rev2 -km elora -e HLC_TFT_DISPLAY=1 -e TARGET=elora_left_tft
mkdir -p "$root/firmware"
cp "$vendor/elora_left_tft.uf2" "$root/firmware/elora_left_tft.uf2"
(cd "$root" && shasum -a 256 firmware/elora_left_tft.uf2 > firmware/elora_left_tft.sha256)
qmk compile -kb splitkb/halcyon/elora/rev2 -km elora -e HLC_ENCODER_REV2=1 -e TARGET=elora_right_encoder_rev2
cp "$vendor/elora_right_encoder_rev2.uf2" "$root/firmware/elora_right_encoder_rev2.uf2"
(cd "$root" && shasum -a 256 firmware/elora_right_encoder_rev2.uf2 > firmware/elora_right_encoder_rev2.sha256)

cp "$vendor/elora_left_tft.uf2" "$root/.build/firmware/"
cp "$vendor/elora_right_encoder_rev2.uf2" "$root/.build/firmware/"
echo "Firmware in $root/.build/firmware"
