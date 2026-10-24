#!/usr/bin/env bash
set -euo pipefail

root="$(cd "$(dirname "$0")/.." && pwd)"
qmk_root="${QMK_HOME:-$HOME/src/qmk_firmware}"
keymap_dir="$root/src"

if [[ ! -d "$qmk_root/tests/test_common" ]]; then
  echo "Set QMK_HOME to a QMK firmware checkout with C unit tests." >&2
  exit 1
fi

for suite in tap_hold keylog; do
  # Rebuild from scratch, or a renamed source lingers and compiles.
  test_dest="$qmk_root/tests/elora_$suite"
  rm -rf "$test_dest"
  mkdir -p "$test_dest"
  cp "$root/tests/qmk_$suite/"* "$test_dest/"
  cp "$keymap_dir/$suite.c" "$keymap_dir/$suite.h" "$test_dest/"
  cp "$keymap_dir/tap_hold.h" "$keymap_dir/tap_hold_config.h" "$test_dest/"
  (cd "$qmk_root" && qmk test-c --test "elora_$suite")
done
