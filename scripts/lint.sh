#!/usr/bin/env bash
set -euo pipefail

# Checks src/ with clang-format and clang-tidy. Pass --fix to rewrite files in
# place; without it the script only reports and fails on any finding.
fix=false
[[ "${1:-}" == "--fix" ]] && fix=true

root="$(cd "$(dirname "$0")/.." && pwd)"
cd "$root"

format="${CLANG_FORMAT:-clang-format}"
tidy="${CLANG_TIDY:-clang-tidy}"
for tool in "$format" "$tidy"; do
  command -v "$tool" >/dev/null || {
    echo "Cannot find $tool; try 'brew install llvm' and put its bin on PATH." >&2
    exit 1
  }
done

# smiley.c and smiley.h are generated; scripts/generate_smiley.py owns them.
sources=(src/config.h src/tap_hold_config.h src/tap_hold.h src/tap_hold.c
         src/keylog.h src/keylog.c src/display.c src/keymap.c)

if [[ "$fix" == true ]]; then
  "$format" -i "${sources[@]}"
  echo "clang-format: rewrote ${#sources[@]} files"
else
  "$format" --dry-run --Werror "${sources[@]}"
  echo "clang-format: clean"
fi

# clang-tidy needs the ARM flags the firmware was built with.
if [[ ! -f compile_commands.json ]]; then
  echo "Generating compile_commands.json; build the left firmware first if this fails." >&2
  python3 scripts/generate_compile_commands.py
fi

# Match only this project's own diagnostics, and not the trailing "note:" lines
# that suppressed warnings in QMK's headers leave behind.
status=0
for source in "${sources[@]}"; do
  [[ "$source" == *.c ]] || continue
  if "$tidy" -p . --quiet "$source" 2>/dev/null | grep -E "^$root/src/.*(warning|error):"; then
    status=1
  fi
done
[[ "$status" == 0 ]] && echo "clang-tidy: clean"
exit "$status"
