#!/usr/bin/env python3
"""Add one platform release to the VTLabs Boards Manager index.

Follows the Arduino package index specification:
https://arduino.github.io/arduino-cli/1.5/package_index_json-specification/

Rules enforced:
  * a version already in the index is never replaced (releases are immutable)
  * checksum is SHA-256, size is in bytes, both computed from the real archive
  * the boards list is read from boards.txt, so it cannot drift

Usage:
  make_index.py add --index package_vtlabs_index.json --archive dist/vtlabs-esp32-0.1.0.zip \
      --version 0.1.0 --boards-txt hardware/vtlabs/esp32/boards.txt --url-base https://.../download/v0.1.0
  make_index.py deprecate --index package_vtlabs_index.json --version 0.1.0
  make_index.py check --index package_vtlabs_index.json
"""
import argparse
import hashlib
import json
import os
import re
import sys

PACKAGE_META = {
    "name": "vtlabs",
    "maintainer": "VTLabs",
    "websiteURL": os.environ.get("VTLABS_WEBSITE", "https://github.com/christech1/vtlabs-arduino"),
    "email": os.environ.get("VTLABS_EMAIL", "vtlabsgroup@gmail.com"),
    "help": {"online": os.environ.get("VTLABS_HELP", "https://github.com/christech1/vtlabs-arduino")},
}
ARCH = "esp32"
PLATFORM_NAME = "VTLabs Training Kits"
SEMVER = re.compile(r"^\d+\.\d+\.\d+(-[0-9A-Za-z.-]+)?$")
REQUIRED_PLATFORM_FIELDS = ["name", "architecture", "version", "category", "url",
                            "archiveFileName", "checksum", "size", "boards", "toolsDependencies"]


def load(path):
    if os.path.exists(path):
        with open(path, encoding="utf-8") as f:
            return json.load(f)
    return {"packages": [dict(PACKAGE_META, platforms=[], tools=[])]}


def save(path, data):
    with open(path, "w", encoding="utf-8") as f:
        json.dump(data, f, indent=2, ensure_ascii=False)
        f.write("\n")


def package(data):
    pkgs = [p for p in data["packages"] if p["name"] == "vtlabs"]
    if len(pkgs) != 1:
        sys.exit("index must contain exactly one 'vtlabs' package")
    return pkgs[0]


def board_names(boards_txt):
    found, hidden = [], set()
    with open(boards_txt, encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            m = re.match(r"^([A-Za-z0-9_-]+)\.name=(.+)$", line)
            if m:
                found.append((m.group(1), m.group(2).strip()))
            m = re.match(r"^([A-Za-z0-9_-]+)\.hide=", line)
            if m:
                hidden.add(m.group(1))
    names = [{"name": n} for bid, n in found if bid not in hidden]
    if not names:
        sys.exit(f"no boards found in {boards_txt}")
    return names


def sha256(path):
    h = hashlib.sha256()
    with open(path, "rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def check(data):
    errors = []
    pkg = package(data)
    for k in ("name", "maintainer", "websiteURL", "email", "platforms", "tools"):
        if k not in pkg:
            errors.append(f"package missing '{k}'")
    seen = set()
    for pl in pkg.get("platforms", []):
        v = pl.get("version", "?")
        for k in REQUIRED_PLATFORM_FIELDS:
            if k not in pl:
                errors.append(f"{v}: missing '{k}'")
        if not SEMVER.match(v):
            errors.append(f"{v}: not semantic version")
        if (pl.get("architecture"), v) in seen:
            errors.append(f"{v}: duplicate version")
        seen.add((pl.get("architecture"), v))
        if pl.get("category") != "Contributed":
            errors.append(f"{v}: category must be 'Contributed'")
        if not str(pl.get("checksum", "")).startswith("SHA-256:"):
            errors.append(f"{v}: checksum must be SHA-256:<hex>")
        if not str(pl.get("size", "")).isdigit():
            errors.append(f"{v}: size must be a byte count string")
        if not pl.get("url", "").endswith(pl.get("archiveFileName", "\0")):
            errors.append(f"{v}: url does not end with archiveFileName")
        if "OWNER/REPO" in pl.get("url", ""):
            errors.append(f"{v}: url still contains the OWNER/REPO placeholder")
    return errors


def main():
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest="cmd", required=True)
    a = sub.add_parser("add")
    a.add_argument("--index", required=True)
    a.add_argument("--archive", required=True)
    a.add_argument("--version", required=True)
    a.add_argument("--boards-txt", required=True)
    a.add_argument("--url-base", required=True, help="URL of the folder holding the archive")
    d = sub.add_parser("deprecate")
    d.add_argument("--index", required=True)
    d.add_argument("--version", required=True)
    c = sub.add_parser("check")
    c.add_argument("--index", required=True)
    c.add_argument("--allow-placeholder", action="store_true")
    args = ap.parse_args()

    data = load(args.index)
    pkg = package(data)

    if args.cmd == "add":
        if not SEMVER.match(args.version):
            sys.exit(f"version '{args.version}' is not semantic (X.Y.Z)")
        if any(p["version"] == args.version and p["architecture"] == ARCH for p in pkg["platforms"]):
            sys.exit(f"version {args.version} is already published; releases are immutable - bump the version")
        name = os.path.basename(args.archive)
        entry = {
            "name": PLATFORM_NAME,
            "architecture": ARCH,
            "version": args.version,
            "category": "Contributed",
            "help": PACKAGE_META["help"],
            "url": args.url_base.rstrip("/") + "/" + name,
            "archiveFileName": name,
            "checksum": "SHA-256:" + sha256(args.archive),
            "size": str(os.path.getsize(args.archive)),
            "boards": board_names(args.boards_txt),
            # Tools come from the referenced esp32:esp32 platform, which students
            # install separately (the index format has no platform dependencies).
            "toolsDependencies": [],
        }
        pkg["platforms"].append(entry)
        print(json.dumps(entry, indent=2))
    elif args.cmd == "deprecate":
        hits = [p for p in pkg["platforms"] if p["version"] == args.version]
        if not hits:
            sys.exit(f"version {args.version} not in index")
        for p in hits:
            p["deprecated"] = True
        print(f"marked {args.version} deprecated")

    errors = check(data)
    if args.cmd == "check" and getattr(args, "allow_placeholder", False):
        errors = [e for e in errors if "placeholder" not in e]
    if errors:
        print("\n".join("INDEX ERROR: " + e for e in errors), file=sys.stderr)
        sys.exit(1)
    if args.cmd != "check":
        save(args.index, data)
    print(f"index OK: {len(pkg['platforms'])} release(s)")


if __name__ == "__main__":
    main()
