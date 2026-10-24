#!/usr/bin/env bash
set -euo pipefail

root="$(cd "$(dirname "$0")/.." && pwd)"
volume="/Volumes/RPI-RP2"
# Nothing on the drive says which half is mounted; both bootloaders are
# identical. Hence the prompt, which --yes skips.
assume_yes=false
[[ "${1:-}" == "--yes" ]] && assume_yes=true

cd "$root"
if [[ ! -f "firmware/elora_left_tft.uf2" ]]; then
  echo "Missing firmware/elora_left_tft.uf2; run ./scripts/build.sh first." >&2
  exit 1
fi

if [[ ! -f "$volume/INFO_UF2.TXT" ]]; then
  echo "Left controller bootloader is not mounted at $volume." >&2
  echo "Connect the LEFT half by USB and double-tap its reset button, then rerun this script." >&2
  exit 1
fi

if [[ "$assume_yes" != true ]]; then
  read -r -p "Write the LEFT image to the controller mounted at $volume? [y/N] " reply
  if [[ "$reply" != [yY] ]]; then
    echo "Nothing written." >&2
    exit 1
  fi
fi

cp firmware/elora_left_tft.uf2 "$volume/elora_left_tft.uf2"
echo "Left firmware copied. The RPI-RP2 drive should disappear as the controller restarts."
