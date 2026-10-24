#!/usr/bin/env bash
set -euo pipefail

root="$(cd "$(dirname "$0")/.." && pwd)"
volume="/Volumes/RPI-RP2"
# Nothing on the drive says which half is mounted; both bootloaders are
# identical. Hence the prompt, which --yes skips.
assume_yes=false
[[ "${1:-}" == "--yes" ]] && assume_yes=true

cd "$root"
if [[ ! -f "firmware/elora_right_encoder_rev2.uf2" ]]; then
  echo "Missing firmware/elora_right_encoder_rev2.uf2; run ./scripts/build.sh first." >&2
  exit 1
fi

if [[ ! -f "$volume/INFO_UF2.TXT" ]]; then
  echo "Right controller bootloader is not mounted at $volume." >&2
  echo "Connect the RIGHT half by USB and double-tap its reset button, then rerun this script." >&2
  exit 1
fi

if [[ "$assume_yes" != true ]]; then
  read -r -p "Write the RIGHT image to the controller mounted at $volume? [y/N] " reply
  if [[ "$reply" != [yY] ]]; then
    echo "Nothing written." >&2
    exit 1
  fi
fi

cp firmware/elora_right_encoder_rev2.uf2 "$volume/elora_right_encoder_rev2.uf2"
echo "Right firmware copied. The RPI-RP2 drive should disappear as the controller restarts."
