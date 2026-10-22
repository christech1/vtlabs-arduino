#!/usr/bin/env python3
"""
Compile every VTLabs example and test sketch for every Tools-menu combination it supports.

Each sketch declares in its header comment:
    @requires: none | keypad | matrix | matrix-max7219 | matrix-ws2812 | keypad, matrix
    @optional: keypad | matrix              (compiled with and without)
    @expect-error: <text>                   (test sketches: compile MUST fail with this text)

Usage:
    python3 scripts/compile_examples.py [--fqbn vtlabs:esp32:vtlabs_iot_esp32_r1]
                                        [--jobs 4] [--only L6_] [--report docs/compile-report.md]
                                        [-- extra arduino-cli args, e.g. --config-file x.yaml]
"""
import argparse
import concurrent.futures as cf
import datetime
import hashlib
import os
import re
import shutil
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
EXAMPLES = os.path.join(ROOT, "hardware", "vtlabs", "esp32", "libraries", "VTLabsKit", "examples")
TESTS = os.path.join(ROOT, "tests", "sketches")

# Tools-menu combinations (menu ids/choices come from boards/*.toml)
KEYPAD = "Keypad=pcf8574_4x4"
OPT = {
    "none":       "",
    "K":          KEYPAD,
    "M4":         "LedMatrix=max7219_4in1",
    "W":          "LedMatrix=ws2812_8x8",
    "K+M4":       KEYPAD + ",LedMatrix=max7219_4in1",
    "K+M8":       KEYPAD + ",LedMatrix=max7219_8x8",
    "K+W":        KEYPAD + ",LedMatrix=ws2812_8x8",
}


def combos(requires, optional, expect_error=None):
    if expect_error:
        return ["none"]
    r, o = set(requires), set(optional)
    if not r:
        return ["none", "K+W"] if not o else ["none", "K+M4", "K+W"]
    if r == {"keypad"}:
        return ["K", "K+M4", "K+W", "K+M8"] if "matrix" in o else ["K", "K+M4"]
    if r == {"matrix"}:
        return ["M4", "W", "K+M4", "K+W", "K+M8"] if "keypad" in o else ["M4", "W", "K+M8"]
    if r == {"keypad", "matrix"}:
        return ["K+M4", "K+M8", "K+W"]
    if r == {"matrix-max7219"}:
        return ["M4", "K+M8"]
    if r == {"matrix-ws2812"}:
        return ["W"]
    raise ValueError(f"unknown @requires combination {sorted(r)}")


def parse(ino):
    text = open(ino, encoding="utf-8").read()
    def tag(name):
        m = re.search(r"@%s:\s*(.+)" % name, text)
        return [] if not m else [t.strip() for t in m.group(1).split(",") if t.strip() and t.strip() != "none"]
    exp = re.search(r"@expect-error:\s*(.+)", text)
    m = re.search(r"VTLabs IoT Training Kit - (.+)", text)
    return {"requires": tag("requires"), "optional": tag("optional"),
            "expect_error": exp.group(1).strip() if exp else None,
            "title": m.group(1).strip() if m else os.path.basename(ino),
            "has_tags": "@requires:" in text}


def find_sketches(only):
    out = []
    for base in (EXAMPLES, TESTS):
        for dirpath, _, files in os.walk(base):
            for f in files:
                if f.endswith(".ino") and f[:-4] == os.path.basename(dirpath):
                    if not only or only in dirpath:
                        out.append(os.path.join(dirpath, f))
    return sorted(out)


BUILD_ROOT = os.path.join(tempfile.gettempdir(), "vtlabs-build")


def compile_one(cli_args, fqbn, sketch_dir):
    # arduino-cli derives the build folder from the sketch path only, so parallel builds of the
    # same sketch for different option sets must get their own --build-path.
    key = hashlib.sha1((sketch_dir + "|" + fqbn).encode()).hexdigest()[:16]
    build = os.path.join(BUILD_ROOT, key)
    cmd = ["arduino-cli"] + cli_args + ["compile", "--fqbn", fqbn, "--warnings", "default",
                                        "--build-path", build, sketch_dir]
    p = subprocess.run(cmd, capture_output=True, text=True)
    shutil.rmtree(build, ignore_errors=True)  # ~25 MB per build; keep disks small
    out = p.stdout + p.stderr
    size = re.search(r"Sketch uses (\d+) bytes \((\d+)%\)", out)
    return p.returncode, out, (f"{int(size.group(1)) // 1024} KB ({size.group(2)}%)" if size else "")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--fqbn", default="vtlabs:esp32:vtlabs_iot_esp32_r1")
    ap.add_argument("--jobs", type=int, default=3)
    ap.add_argument("--only", default="")
    ap.add_argument("--report", default="")
    ap.add_argument("--keypad-option", default=KEYPAD, help="menu value for the keypad, e.g. Keypad=gpio_4x4")
    ap.add_argument("--combos", default="", help="comma list of option sets to keep, e.g. K or none,K")
    ap.add_argument("cli_args", nargs="*")
    a = ap.parse_args()

    if a.keypad_option != KEYPAD:
        for k in OPT:
            OPT[k] = OPT[k].replace(KEYPAD, a.keypad_option)
    keep = set(a.combos.split(",")) if a.combos else None
    jobs = []
    problems = []
    for ino in find_sketches(a.only):
        meta = parse(ino)
        if not meta["has_tags"]:
            problems.append(f"{os.path.relpath(ino, ROOT)}: missing @requires tag")
            continue
        for c in combos(meta["requires"], meta["optional"], meta["expect_error"]):
            if keep and c not in keep:
                continue
            fq = a.fqbn + (":" + OPT[c] if OPT[c] else "")
            jobs.append((ino, meta, c, fq))

    results = []
    with cf.ThreadPoolExecutor(max_workers=a.jobs) as ex:
        futs = {ex.submit(compile_one, a.cli_args, fq, os.path.dirname(ino)): (ino, meta, c, fq)
                for ino, meta, c, fq in jobs}
        for fut in cf.as_completed(futs):
            ino, meta, c, fq = futs[fut]
            rc, out, size = fut.result()
            rel = os.path.relpath(os.path.dirname(ino), ROOT)
            if meta["expect_error"]:
                ok = rc != 0 and meta["expect_error"] in out
                detail = "failed as expected" if ok else "expected compile error not seen"
            else:
                ok = rc == 0
                detail = size
            if not ok:
                errs = [l for l in out.splitlines() if "error" in l.lower() and "discovery" not in l][:6]
                detail += "\n      " + "\n      ".join(errs)
            print(f"{'PASS' if ok else 'FAIL'}  {c:<5} {rel}  {detail}", flush=True)
            results.append((rel, meta["title"], c, ok, size if not meta["expect_error"] else "expected error"))

    results.sort()
    passed = sum(r[3] for r in results)
    print("----")
    for p in problems:
        print("PROBLEM", p)
    print(f"{passed}/{len(results)} compilations passed, {len({r[0] for r in results})} sketches")

    if a.report:
        cli = subprocess.run(["arduino-cli"] + a.cli_args + ["version"], capture_output=True, text=True).stdout.strip()
        L = ["# Compile report", "",
             f"Generated {datetime.datetime.now().strftime('%Y-%m-%d %H:%M')} by `scripts/compile_examples.py`.",
             f"Board: `{a.fqbn}`  |  {cli}", "",
             "Compilation only proves the code builds for the selected target. It does NOT prove the "
             "hardware works - see docs/hardware-validation-checklist.md.", "",
             "Option sets: " + ", ".join(f"`{k}` = `{v or 'Keypad: Not fitted, LED Matrix: Not fitted'}`" for k, v in OPT.items()), "",
             f"**{passed}/{len(results)} passed**", "",
             "| Sketch | Option set | Result | Flash used |", "|---|---|---|---|"]
        for rel, title, c, ok, size in results:
            L.append(f"| {title} | {c} | {'PASS' if ok else '**FAIL**'} | {size} |")
        with open(os.path.join(ROOT, a.report), "w", encoding="utf-8", newline="\n") as f:
            f.write("\n".join(L) + "\n")
        print("report written to", a.report)
    sys.exit(0 if passed == len(results) and not problems else 1)


if __name__ == "__main__":
    main()
