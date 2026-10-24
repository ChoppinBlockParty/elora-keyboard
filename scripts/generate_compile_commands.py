#!/usr/bin/env python3
"""Generate clangd commands for the project sources from QMK's left build flags."""

import argparse
import json
import os
import shlex
import shutil
import subprocess
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=Path(__file__).resolve().parent.parent)
    parser.add_argument("--output", type=Path)
    parser.add_argument("--qmk-home", type=Path, default=os.environ.get("QMK_HOME"))
    parser.add_argument("--arm-gcc", default="arm-none-eabi-gcc")
    args = parser.parse_args()

    root = args.root.resolve()
    qmk = (args.qmk_home or root / ".build/qmk_firmware").expanduser().resolve()
    cflags = qmk / ".build/obj_elora_left_tft/cflags.txt"
    if not cflags.is_file():
        parser.error(f"Build the left firmware first; missing {cflags}")

    tokens = shlex.split(cflags.read_text())
    compiler = shutil.which(args.arm_gcc)
    if not compiler:
        parser.error(f"Cannot find {args.arm_gcc}; put the ARM toolchain on PATH or use --arm-gcc")
    # Query GCC rather than assuming a Homebrew installation layout. Clang's
    # ARM target needs the same C library headers used to build the firmware.
    architecture = [token for token in tokens if token.startswith("-mcpu=") or token == "-mthumb"]
    probe = subprocess.run(
        [compiler, *architecture, "-E", "-v", "-x", "c", "-"],
        input="", capture_output=True, text=True, check=True,
    )
    include_paths = []
    in_search_list = False
    for line in probe.stderr.splitlines():
        if line == "#include <...> search starts here:":
            in_search_list = True
        elif line == "End of search list.":
            in_search_list = False
        elif in_search_list:
            include_paths.append(Path(line.strip()).resolve())
    if not include_paths:
        parser.error(f"Cannot discover ARM system headers from {compiler}")
    # Apple clang needs this Pico SDK macro when parsing QMK's ARM headers.
    flags = [
        "--target=arm-none-eabi", *architecture, "-std=gnu11",
        "-Wno-unknown-attributes", "-D__always_inline=inline", f"-I{root / 'src'}",
    ]
    for include in include_paths:
        flags.extend(("-isystem", str(include)))
    vendor_keymap = root / ".build/splitkb-qmk-userspace/keyboards/splitkb/halcyon/elora/keymaps/elora"
    index = 0
    while index < len(tokens):
        token = tokens[index]
        token = token.replace(str(vendor_keymap), str(root / "src"))
        if token.startswith(("-I", "-D", "-O")) or token == "-fshort-wchar":
            flags.append(token)
        elif token == "-include":
            index += 1
            included = tokens[index].replace(str(vendor_keymap / "config.h"), str(root / "src/config.h"))
            flags.extend((token, included))
        index += 1

    sources = sorted((root / "src").glob("*.c"))
    if not sources:
        parser.error(f"No C sources found in {root / 'src'}")
    database = [
        {
            "directory": str(qmk),
            "file": str(source),
            "arguments": ["clang", *flags, "-c", str(source)],
        }
        for source in sources
    ]
    output = args.output or root / "compile_commands.json"
    output.write_text(json.dumps(database, indent=2) + "\n")
    print(f"Wrote {len(database)} commands to {output}")


if __name__ == "__main__":
    main()
