#!/usr/bin/env bash
set -euo pipefail

root="$(cd "$(dirname "$0")/.." && pwd)"
volume="/Volumes/RPI-RP2"

cd "$root"
shasum -a 256 -c firmware/elora_right_encoder_rev2.sha256

if [[ ! -f "$volume/INFO_UF2.TXT" ]]; then
  echo "Right controller bootloader is not mounted at $volume." >&2
  echo "Connect the RIGHT half by USB and double-tap its reset button, then rerun this script." >&2
  exit 1
fi

cp firmware/elora_right_encoder_rev2.uf2 "$volume/elora_right_encoder_rev2.uf2"
echo "Right firmware copied. The RPI-RP2 drive should disappear as the controller restarts."
