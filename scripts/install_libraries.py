#!/usr/bin/env python3
"""Install every library from libraries.txt with arduino-cli (same versions CI uses).

    python3 scripts/install_libraries.py [-- extra arduino-cli args]

Arduino IDE users can instead use Tools > Manage Libraries and search each name,
or rely on the Boards Manager (IDE 2.4+ installs them with the VTLabs platform).
"""
import os
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
libs = [l.strip() for l in open(os.path.join(ROOT, "libraries.txt"), encoding="utf-8")
        if l.strip() and not l.startswith("#")]
extra = sys.argv[2:] if len(sys.argv) > 1 and sys.argv[1] == "--" else sys.argv[1:]
subprocess.run(["arduino-cli"] + extra + ["lib", "update-index"], check=True)
for lib in libs:
    print("installing", lib, flush=True)
    subprocess.run(["arduino-cli"] + extra + ["lib", "install", lib], check=True)
print(f"{len(libs)} libraries installed")
