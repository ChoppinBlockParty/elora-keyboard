#!/usr/bin/env bash
set -euo pipefail

root="$(cd "$(dirname "$0")/.." && pwd)"
volume="/Volumes/RPI-RP2"

cd "$root"
shasum -a 256 -c firmware/elora_left_tft.sha256

if [[ ! -f "$volume/INFO_UF2.TXT" ]]; then
  echo "Left controller bootloader is not mounted at $volume." >&2
  echo "Connect the LEFT half by USB and double-tap its reset button, then rerun this script." >&2
  exit 1
fi

cp firmware/elora_left_tft.uf2 "$volume/elora_left_tft.uf2"
echo "Left firmware copied. The RPI-RP2 drive should disappear as the controller restarts."
