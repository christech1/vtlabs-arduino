#!/usr/bin/env python3
"""
VTLabs board tool - one hardware profile (boards/*.toml) -> everything Arduino needs.

  python3 tools/vtlabs_board.py check             validate all profiles, print problems
  python3 tools/vtlabs_board.py generate          validate, then write generated files
  python3 tools/vtlabs_board.py generate --check  fail if generated files are out of date (CI)
  python3 tools/vtlabs_board.py list              show boards and their pins
  python3 tools/vtlabs_board.py new --from vtlabs_iot_esp32_r1 --id myschool_esp32_r1 --name "My School ESP32"

Generated (never edit these by hand):
  hardware/vtlabs/esp32/boards.txt
  hardware/vtlabs/esp32/variants/<id>/pins_arduino.h
  hardware/vtlabs/esp32/variants/<id>/vtlabs_board.h
  docs/pinmaps/<id>.md

Requires Python 3.11+ (tomllib). On older Python: pip install tomli
"""
import argparse
import itertools
import os
import re
import sys

try:
    import tomllib
except ModuleNotFoundError:  # Python < 3.11
    try:
        import tomli as tomllib
    except ModuleNotFoundError:
        sys.exit("Python 3.11+ is required (or: pip install tomli)")

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
PROFILES = os.path.join(ROOT, "boards")
PLATFORM = os.path.join(ROOT, "hardware", "vtlabs", "esp32")
PINDOCS = os.path.join(ROOT, "docs", "pinmaps")
NAME_RE = re.compile(r"^[A-Z][A-Z0-9_]*$")
ID_RE = re.compile(r"^[a-z][a-z0-9_]*$")

# --------------------------------------------------------------------------
# MCU family facts. Source: ESP32 datasheet + espressif/arduino-esp32 3.3.12
# (variants/esp32/pins_arduino.h, boards.txt "ESP32 Dev Module").
# To support another family (e.g. esp32s3) add an entry here.
# --------------------------------------------------------------------------
FAMILIES = {
    "esp32": {
        "build": {"tarch": "xtensa", "target": "esp32", "mcu": "esp32",
                  "bootloader_addr": "0x1000", "maximum_data_size": 327680},
        "core_min": (3, 3, 0), "core_max_excl": (3, 4, 0),
        "gpios": set(range(0, 40)) - {20, 24, 28, 29, 30, 31},
        "flash": {6, 7, 8, 9, 10, 11},
        "input_only": {34, 35, 36, 37, 38, 39},
        "strapping": {0: "must be HIGH at boot (BOOT button)",
                      2: "must be LOW/floating at boot for serial flashing",
                      5: "controls SDIO timing at boot",
                      12: "MTDI - if HIGH at boot the flash voltage becomes 1.8 V and boot fails",
                      15: "MTDO - LOW at boot silences the boot log"},
        "adc1": {32, 33, 34, 35, 36, 37, 38, 39},
        "adc2": {0, 2, 4, 12, 13, 14, 15, 25, 26, 27},
        "boot_pwm": {14: "outputs a PWM signal during boot", 15: "outputs a PWM signal during boot",
                     1: "TX0 boot log", 3: "RX0"},
        "analog_aliases": {"A0": 36, "A3": 39, "A4": 32, "A5": 33, "A6": 34, "A7": 35, "A10": 4,
                           "A11": 0, "A12": 2, "A13": 15, "A14": 13, "A15": 12, "A16": 14,
                           "A17": 27, "A18": 25, "A19": 26},
        "touch_aliases": {"T0": 4, "T1": 0, "T2": 2, "T3": 15, "T4": 13, "T5": 12, "T6": 14,
                          "T7": 27, "T8": 33, "T9": 32},
        "dac_aliases": {"DAC1": 25, "DAC2": 26},
        "psram_defines": "-DBOARD_HAS_PSRAM -mfix-esp32-psram-cache-issue -mfix-esp32-psram-cache-strategy=memw",
        # menu id: (csv name, label, max app size, minimum flash MB)
        "partitions": {
            "default": ("default", "Default 4MB with spiffs (1.2MB APP/1.5MB SPIFFS)", 1310720, 4),
            "defaultffat": ("default_ffat", "Default 4MB with ffat (1.2MB APP/1.5MB FATFS)", 1310720, 4),
            "minimal": ("minimal", "Minimal (1.3MB APP/700KB SPIFFS)", 1310720, 4),
            "no_fs": ("no_fs", "No FS 4MB (2MB APP x2)", 2031616, 4),
            "no_ota": ("no_ota", "No OTA (2MB APP/2MB SPIFFS)", 2097152, 4),
            "noota_3g": ("noota_3g", "No OTA (1MB APP/3MB SPIFFS)", 1048576, 4),
            "min_spiffs": ("min_spiffs", "Minimal SPIFFS (1.9MB APP with OTA/128KB SPIFFS)", 1966080, 4),
            "huge_app": ("huge_app", "Huge APP (3MB No OTA/1MB SPIFFS)", 3145728, 4),
            "default_8MB": ("default_8MB", "8M with spiffs (3MB APP/1.5MB SPIFFS)", 3342336, 8),
            "app3M_fat9M_16MB": ("app3M_fat9M_16MB", "16M Flash (3MB APP/9.9MB FATFS)", 3145728, 16),
            "fatflash": ("ffat", "16M Flash (2MB APP/12.5MB FATFS)", 2097152, 16),
        },
        "cpu_freqs": {240: "240MHz (WiFi/BT)", 160: "160MHz (WiFi/BT)", 80: "80MHz (WiFi/BT)",
                      40: "40MHz (40MHz XTAL)", 20: "20MHz (40MHz XTAL)", 10: "10MHz (40MHz XTAL)"},
    }
}
BUS_KEYS = ["uart0_tx", "uart0_rx", "i2c_sda", "i2c_scl", "spi_sck", "spi_miso", "spi_mosi",
            "spi_ss", "uart2_rx", "uart2_tx"]
BUS_MODES = {"uart0_tx": "out", "uart0_rx": "in", "i2c_sda": "io", "i2c_scl": "io",
             "spi_sck": "out", "spi_miso": "in", "spi_mosi": "out", "spi_ss": "out",
             "uart2_rx": "in", "uart2_tx": "out"}
MODES = {"out", "in", "io", "analog", "pwm"}
# Names the VTLabsKit curriculum uses. A profile must define all of them (use gpio = -1
# for "not fitted") or some examples will not compile.
CURRICULUM = {
    "pins": ["LED_BUILTIN", "LED_RED", "LED_GREEN", "LED_YELLOW", "BUZZER", "RELAY", "BTN1", "BTN2",
             "SWITCH", "POT", "LDR", "DHT", "PIR", "SD_CS", "RS485_DE"],
    "settings": ["RELAY_ACTIVE_HIGH", "DHT_TYPE", "OLED_WIDTH", "OLED_HEIGHT", "ADC_VREF_MV"],
    "i2c": ["OLED", "BME280"],
    "bus": BUS_KEYS,
}
INPUT_MODES = {"internal": "INPUT_PULLUP", "internal-down": "INPUT_PULLDOWN", "external": "INPUT", "none": "INPUT"}


# --------------------------------------------------------------------------
def load_profiles(directory=PROFILES):
    profiles = []
    for fn in sorted(os.listdir(directory)):
        if fn.endswith(".toml"):
            with open(os.path.join(directory, fn), "rb") as f:
                p = tomllib.load(f)
            p["_file"] = os.path.relpath(os.path.join(directory, fn), ROOT)
            profiles.append(p)
    return profiles


def c_value(v):
    """TOML value -> C token. Strings are emitted as-is (write '"text"' for a C string)."""
    if isinstance(v, bool):
        return "1" if v else "0"
    return str(v)


def allocations(p, combo):
    """All GPIO users for one combination of option choices."""
    out = []
    for k in BUS_KEYS:
        if k in p.get("bus", {}):
            out.append({"name": k.upper(), "gpio": p["bus"][k], "mode": BUS_MODES[k], "src": "bus"})
    for n, d in p.get("pins", {}).items():
        out.append(dict(d, name=n, src="pins"))
    for opt, ch in combo:
        for n, d in ch.get("pins", {}).items():
            out.append(dict(d, name=n, src=f"{opt['menu']}={ch['id']}"))
    return out


def option_combos(p):
    opts = p.get("option", [])
    lists = [[(o, c) for c in o["choice"]] for o in opts]
    return [list(c) for c in itertools.product(*lists)] if lists else [[]]


def validate(p):
    errors, warnings = [], []
    b = p.get("board", {})
    pid = b.get("id", "?")
    E = lambda m: errors.append(f"{p['_file']}: {m}")
    W = lambda m: warnings.append(f"{p['_file']}: {m}")

    for key in ("id", "name", "hw_rev", "family"):
        if key not in b:
            E(f"[board] missing '{key}'")
    if not ID_RE.match(pid):
        E(f"board id '{pid}' must be lowercase letters, digits, underscores")
    fam = FAMILIES.get(b.get("family"))
    if not fam:
        E(f"unknown family '{b.get('family')}' (known: {', '.join(FAMILIES)})")
        return errors, warnings

    bd = p.get("build", {})
    flash_mb = int(str(bd.get("flash_size", "4MB")).rstrip("MB"))
    for part in bd.get("partitions", ["default"]):
        if part not in fam["partitions"]:
            E(f"unknown partition scheme '{part}' (known: {', '.join(fam['partitions'])})")
        elif fam["partitions"][part][3] > flash_mb:
            E(f"partition '{part}' needs {fam['partitions'][part][3]}MB flash, board has {flash_mb}MB")
    for f in bd.get("cpu_freqs", [240]):
        if f not in fam["cpu_freqs"]:
            E(f"cpu frequency {f} not supported")

    for n in list(p.get("pins", {})) + list(p.get("settings", {})) + list(p.get("i2c", {})):
        if not NAME_RE.match(n):
            E(f"name '{n}' must be UPPER_CASE (letters, digits, _)")
    menus = set()
    for o in p.get("option", []):
        if not re.match(r"^[A-Za-z][A-Za-z0-9_]*$", o.get("menu", "")):
            E(f"option menu '{o.get('menu')}' must be letters/digits/_")
        if o["menu"] in menus:
            E(f"option menu '{o['menu']}' defined twice")
        menus.add(o["menu"])
        ids = [c["id"] for c in o.get("choice", [])]
        if len(ids) != len(set(ids)) or not ids:
            E(f"option '{o['menu']}' needs unique choice ids")
        for c in o.get("choice", []):
            for n in c.get("pins", {}):
                if not NAME_RE.match(n):
                    E(f"option pin '{n}' must be UPPER_CASE")

    seen_msgs = {}
    for combo in option_combos(p):
        used = {}
        label = ", ".join(f"{o['menu']}={c['id']}" for o, c in combo) or "base"
        for a in allocations(p, combo):
            g, mode, name = a.get("gpio"), a.get("mode", "io"), a["name"]
            if not isinstance(g, int):
                E(f"{name}: gpio must be an integer (use -1 for not connected)")
                continue
            if mode not in MODES:
                E(f"{name}: mode '{mode}' must be one of {sorted(MODES)}")
            if g == -1:
                continue
            msgs = []
            if g not in fam["gpios"]:
                msgs.append(("E", f"{name}: GPIO{g} does not exist on {b['family']}"))
            elif g in fam["flash"]:
                msgs.append(("E", f"{name}: GPIO{g} is wired to the SPI flash - never use 6..11"))
            else:
                if g in fam["input_only"] and mode in ("out", "pwm", "io"):
                    msgs.append(("E", f"{name}: GPIO{g} is input-only, cannot be '{mode}'"))
                if g in fam["input_only"] and a.get("pull") == "internal":
                    msgs.append(("E", f"{name}: GPIO{g} has no internal pull-up/down; use pull = \"external\""))
                if mode == "analog" and g not in fam["adc1"] | fam["adc2"]:
                    msgs.append(("E", f"{name}: GPIO{g} has no ADC"))
                if mode == "analog" and g in fam["adc2"]:
                    msgs.append(("W", f"{name}: GPIO{g} is ADC2 - analogRead() fails while Wi-Fi is on"))
                if g in fam["strapping"] and a["src"] != "bus":
                    msgs.append(("W", f"{name}: GPIO{g} is a strapping pin ({fam['strapping'][g]})"))
                if g == 12 and mode in ("in", "io") and a.get("pull") in ("external", "internal"):
                    msgs.append(("E", f"{name}: GPIO12 with a pull-up stops the ESP32 from booting"))
                if g in (14, 15) and mode == "out" and name.startswith("RELAY"):
                    msgs.append(("W", f"{name}: GPIO{g} {fam['boot_pwm'][g]} - relay may click at reset"))
            if g in used:
                other, other_shares = used[g]
                if a.get("shares") != other and other_shares != name:
                    msgs.append(("E", f"GPIO{g} used twice: {other} and {name} "
                                      f"(add shares = \"{other}\" to {name} if this is intended)"))
            else:
                used[g] = (name, a.get("shares"))
            for kind, m in msgs:
                seen_msgs.setdefault(m, [kind, []])[1].append(label)
    n_combos = len(option_combos(p))
    for m, (kind, labels) in seen_msgs.items():
        where = "" if len(labels) == n_combos else f"  [with {'; '.join(sorted(set(labels)))}]"
        (E if kind == "E" else W)(m + where)
    for section, names in CURRICULUM.items():
        missing = [n for n in names if n not in p.get(section, {})]
        if missing:
            E(f"[{section}] is missing {', '.join(missing)} - the curriculum examples use these names "
              f"(set gpio = -1 if the part is not fitted)")
    nc = [n for n, d in p.get("pins", {}).items() if d.get("gpio") == -1 and n != "RS485_DE"]
    if nc:
        W(f"not fitted on this board (examples using them will compile but those parts do nothing): {', '.join(nc)}")
    unverified = [n for n, d in p.get("pins", {}).items() if not d.get("verified", False)]
    if unverified:
        W(f"{len(unverified)} pin(s) not yet verified on hardware: {', '.join(unverified)}")
    return errors, warnings


# --------------------------------------------------------------------------
GEN_NOTE = "GENERATED by tools/vtlabs_board.py from {src} - DO NOT EDIT. Edit the .toml and run: python3 tools/vtlabs_board.py generate"


def gen_vtlabs_board_h(p):
    b, fam_name = p["board"], p["board"]["family"]
    L = ["/*", " * " + GEN_NOTE.format(src=p["_file"].replace("\\", "/")), " *",
         f" * {b['name']}", " * Board-specific hardware description used by the VTLabsKit library and examples.",
         " * Any VTL_ value can be overridden in a sketch by #define-ing it BEFORE #include <VTLabsKit.h>.",
         " */", "#pragma once", "",
         f'#define VTL_BOARD_ID "{b["id"]}"', f'#define VTL_BOARD_NAME "{b["name"]}"',
         f"#define VTL_HW_REV {b['hw_rev']}", f'#define VTL_MCU_FAMILY "{fam_name}"',
         f"#define VTL_BOARD_VERIFIED {c_value(b.get('verified', False))}", "",
         "/* constants used by option settings */",
         "#define VTL_KEYPAD_BACKEND_GPIO 1", "#define VTL_KEYPAD_BACKEND_PCF8574 2",
         "#define VTL_MATRIX_DRIVER_MAX7219 1", "#define VTL_MATRIX_DRIVER_WS2812 2", "",
         "/* ---- bus pins ---- */"]

    def define(name, value, comment=""):
        c = f"  /* {comment} */" if comment else ""
        return [f"#ifndef {name}", f"#define {name} {value}{c}", "#endif"]

    for k in BUS_KEYS:
        if k in p.get("bus", {}):
            L += define(f"VTL_PIN_{k.upper()}", p["bus"][k])
    L += ["", "/* ---- on-board pins ---- */"]
    for n, d in p.get("pins", {}).items():
        flag = "" if d.get("verified") else " [unverified]"
        L += define(f"VTL_PIN_{n}", d["gpio"], f"{d.get('mode', 'io')}: {d.get('desc', '')}{flag}")
        L += define(f"VTL_PIN_{n}_ACTIVE_LOW", c_value(d.get("active_low", False)))
        if d.get("mode") == "in":
            L += define(f"VTL_PIN_{n}_MODE", INPUT_MODES.get(d.get("pull", "none"), "INPUT"),
                        "pinMode() value for this input")
    L += ["", "/* ---- settings ---- */"]
    for n, v in p.get("settings", {}).items():
        L += define(f"VTL_{n}", c_value(v))
    L += ["", "/* ---- I2C addresses ---- */"]
    for n, d in p.get("i2c", {}).items():
        L += define(f"VTL_I2C_{n}", f"0x{d['addr']:02X}", d.get("desc", ""))

    for o in p.get("option", []):
        menu = o["menu"]
        sym = lambda c: f"VTL_OPT_{menu.upper()}_{c['id'].upper()}"
        L += ["", f"/* ---- add-on: {o.get('label', menu)} (Arduino IDE: Tools > {o.get('label', menu)}) ---- */"]
        L += ["#if " + " && ".join(f"!defined({sym(c)})" for c in o["choice"]),
              f"#define {sym(o['choice'][0])}  /* default when no menu value is passed */", "#endif"]
        for i, c in enumerate(o["choice"]):
            L.append(("#if " if i == 0 else "#elif ") + f"defined({sym(c)})")
            L.append(f'#define VTL_{menu.upper()}_OPTION "{c["id"]}"')
            L.append(f'#define VTL_{menu.upper()}_OPTION_LABEL "{c["label"]}"')
            for n, v in c.get("defines", {}).items():
                L += define(f"VTL_{n}", c_value(v))
            for n, d in c.get("pins", {}).items():
                L += define(f"VTL_PIN_{n}", d["gpio"], f"{d.get('mode', 'io')}: {d.get('desc', '')}")
        L.append("#endif")
    return "\n".join(L) + "\n"


def gen_pins_arduino_h(p):
    fam = FAMILIES[p["board"]["family"]]
    bus, pins = p.get("bus", {}), p.get("pins", {})
    lo, hi = fam["core_min"], fam["core_max_excl"]
    L = ["/*", " * " + GEN_NOTE.format(src=p["_file"].replace("\\", "/")), " *",
         " * Standard Arduino pin names expected by the Espressif core and libraries.",
         " * Kit peripherals are in vtlabs_board.h (included by <VTLabsKit.h>).", " */",
         "#ifndef Pins_Arduino_h", "#define Pins_Arduino_h", "", "#include <stdint.h>",
         '#include "esp_arduino_version.h"', "",
         "/* The Boards Manager cannot make vtlabs:esp32 depend on esp32:esp32, so check here. */",
         f"#if (ESP_ARDUINO_VERSION < ESP_ARDUINO_VERSION_VAL({lo[0]}, {lo[1]}, {lo[2]})) || "
         f"(ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL({hi[0]}, {hi[1]}, {hi[2]}))",
         f"#error \"VTLabs boards need the 'esp32 by Espressif Systems' platform version {lo[0]}.{lo[1]}.x. Install it from Tools > Board > Boards Manager.\"",
         "#endif", ""]
    if "LED_BUILTIN" in pins and pins["LED_BUILTIN"]["gpio"] >= 0:
        L += [f"static const uint8_t LED_BUILTIN = {pins['LED_BUILTIN']['gpio']};",
              "#define BUILTIN_LED LED_BUILTIN  // backward compatibility",
              "#define LED_BUILTIN LED_BUILTIN  // allow testing #ifdef LED_BUILTIN", ""]
    for a, k in (("TX", "uart0_tx"), ("RX", "uart0_rx"), ("SDA", "i2c_sda"), ("SCL", "i2c_scl"),
                 ("SS", "spi_ss"), ("MOSI", "spi_mosi"), ("MISO", "spi_miso"), ("SCK", "spi_sck")):
        if k in bus:
            L.append(f"static const uint8_t {a} = {bus[k]};")
    if "uart2_rx" in bus:
        L += ["", "/* Serial2 default pins used by the core */",
              f"#define RX2 {bus['uart2_rx']}", f"#define TX2 {bus['uart2_tx']}"]
    L.append("")
    for group in ("analog_aliases", "touch_aliases", "dac_aliases"):
        for a, g in fam[group].items():
            L.append(f"static const uint8_t {a} = {g};")
        L.append("")
    L.append("#endif /* Pins_Arduino_h */")
    return "\n".join(L) + "\n"


def gen_boards_txt(profiles):
    L = ["# " + GEN_NOTE.format(src="boards/*.toml"), "#",
         "# VTLabs Training Kits - board definitions (platform vtlabs:esp32).",
         "# Every board uses the official Espressif core through build.core=esp32:esp32.", ""]
    menus = {"UploadSpeed": "Upload Speed", "CPUFreq": "CPU Frequency", "PartitionScheme": "Partition Scheme"}
    for p in profiles:
        for o in p.get("option", []):
            menus.setdefault(o["menu"], o.get("label", o["menu"]))
    menus.update({"DebugLevel": "Core Debug Level", "EraseFlash": "Erase All Flash Before Sketch Upload"})
    L += [f"menu.{k}={v}" for k, v in menus.items()] + [""]

    for p in profiles:
        b, bd = p["board"], p.get("build", {})
        fam = FAMILIES[b["family"]]
        i = b["id"]
        P = lambda k, v: L.append(f"{i}.{k}={v}")
        L += ["#" * 78, f"# {b['name']}   (profile: {p['_file']})", "#" * 78]
        P("name", b["name"])
        if b.get("hide"):
            P("hide", "")
        P("build.core", "esp32:esp32")
        P("build.variant", i)
        P("build.board", i.upper())
        for k, v in fam["build"].items():
            if k != "maximum_data_size":
                P(f"build.{k}", v)
        P("build.flash_size", bd.get("flash_size", "4MB"))
        P("build.flash_mode", bd.get("flash_mode", "dio"))
        P("build.boot", bd.get("flash_mode", "dio"))
        P("build.flash_freq", bd.get("flash_freq", "40m"))
        opt_props = " ".join("{build.vtl_opt_%s}" % o["menu"].lower() for o in p.get("option", []))
        psram = fam["psram_defines"] + " " if bd.get("psram") else ""
        P("build.defines", f"{psram}-DVTLABS_BOARD=1 -DVTLABS_HW_REV={b['hw_rev']} {opt_props}".strip())
        for o in p.get("option", []):
            P(f"build.vtl_opt_{o['menu'].lower()}", "")
        for k in ("extra_libs", "loop_core", "event_core"):
            P(f"build.{k}", "")
        P("bootloader.tool", "esptool_py")
        P("bootloader.tool.default", "esptool_py")
        P("upload.tool", "esptool_py")
        P("upload.tool.default", "esptool_py")
        P("upload.tool.network", "esp_ota")
        P("upload.maximum_data_size", fam["build"]["maximum_data_size"])
        for k in ("flags", "erase_cmd", "extra_flags"):
            P(f"upload.{k}", "")
        P("serial.disableDTR", "true")
        P("serial.disableRTS", "true")
        P("monitor_port.serial.baudrate", bd.get("monitor_baud", 115200))
        L.append("")
        for s in bd.get("upload_speeds", [921600, 115200]):
            P(f"menu.UploadSpeed.{s}", s)
            P(f"menu.UploadSpeed.{s}.upload.speed", s)
        for f in bd.get("cpu_freqs", [240]):
            P(f"menu.CPUFreq.{f}", fam["cpu_freqs"][f])
            P(f"menu.CPUFreq.{f}.build.f_cpu", f"{f}000000L")
        for part in bd.get("partitions", ["default"]):
            csv, label, mx, _ = fam["partitions"][part]
            P(f"menu.PartitionScheme.{part}", label)
            P(f"menu.PartitionScheme.{part}.build.partitions", csv)
            P(f"menu.PartitionScheme.{part}.upload.maximum_size", mx)
        for o in p.get("option", []):
            for c in o["choice"]:
                P(f"menu.{o['menu']}.{c['id']}", c["label"])
                P(f"menu.{o['menu']}.{c['id']}.build.vtl_opt_{o['menu'].lower()}",
                  f"-DVTL_OPT_{o['menu'].upper()}_{c['id'].upper()}")
        for lvl, name in enumerate(["none", "error", "warn", "info", "debug", "verbose"]):
            P(f"menu.DebugLevel.{name}", name.capitalize())
            P(f"menu.DebugLevel.{name}.build.code_debug", lvl)
        P("menu.EraseFlash.none", "Disabled")
        P("menu.EraseFlash.none.upload.erase_cmd", "")
        P("menu.EraseFlash.all", "Enabled")
        P("menu.EraseFlash.all.upload.erase_cmd", "-e")
        L.append("")
    return "\n".join(L)


def gen_pin_doc(p, warnings):
    b = p["board"]
    fam = FAMILIES[b["family"]]
    L = [f"# Pin map - {b['name']}", "",
         f"<!-- {GEN_NOTE.format(src=p['_file'])} -->", "",
         f"FQBN: `vtlabs:esp32:{b['id']}`  |  Module: {b.get('module', '?')}  |  Hardware verified: **{'yes' if b.get('verified') else 'NO - design assumption'}**", "",
         "| GPIO | Code name | Mode | Function | Verified | Notes |", "|---:|---|---|---|---|---|"]
    rows = []
    for k in BUS_KEYS:
        if k in p.get("bus", {}):
            rows.append((p["bus"][k], f"VTL_PIN_{k.upper()}", BUS_MODES[k], "Bus: " + k.replace("_", " ").upper(), "-", ""))
    for n, d in p.get("pins", {}).items():
        rows.append((d["gpio"], f"VTL_PIN_{n}", d.get("mode", "io"), d.get("desc", ""), "yes" if d.get("verified") else "no",
                     "active LOW" if d.get("active_low") else ""))
    for o in p.get("option", []):
        for c in o["choice"]:
            for n, d in c.get("pins", {}).items():
                rows.append((d["gpio"], f"VTL_PIN_{n}", d.get("mode", "io"), d.get("desc", ""), "no",
                             f"only with Tools > {o.get('label', o['menu'])} = {c['label']}"))
    for g, n, m, desc, v, note in sorted(rows, key=lambda r: (r[0] < 0, r[0])):
        extra = []
        if g in fam["strapping"]:
            extra.append("strapping pin")
        if g in fam["input_only"]:
            extra.append("input only")
        if g in fam["adc2"] and m == "analog":
            extra.append("ADC2: not with Wi-Fi")
        note = "; ".join(x for x in [note] + extra if x)
        L.append(f"| {'NC' if g < 0 else g} | `{n}` | {m} | {desc} | {v} | {note} |")
    used = {r[0] for r in rows}
    free = sorted(g for g in fam["gpios"] - fam["flash"] if g not in used)
    L += ["", f"Free GPIOs: {', '.join(map(str, free)) if free else 'none'}  ",
          f"Never use: GPIO {', '.join(map(str, sorted(fam['flash'])))} (internal flash).", ""]
    if p.get("i2c"):
        L += ["## I2C devices", "", "| Address | Code name | Device |", "|---|---|---|"]
        L += [f"| 0x{d['addr']:02X} | `VTL_I2C_{n}` | {d.get('desc', '')} |" for n, d in p["i2c"].items()]
        L.append("")
    for o in p.get("option", []):
        L += [f"## Tools > {o.get('label', o['menu'])}", "", "| Menu choice | Macro defined | Settings |", "|---|---|---|"]
        for c in o["choice"]:
            s = ", ".join(f"`VTL_{k}={c_value(v)}`" for k, v in c.get("defines", {}).items())
            L.append(f"| {c['label']} | `VTL_OPT_{o['menu'].upper()}_{c['id'].upper()}` | {s} |")
        L.append("")
    if warnings:
        L += ["## Checker warnings", ""] + [f"- {w.split(': ', 1)[1]}" for w in warnings] + [""]
    return "\n".join(L)


# --------------------------------------------------------------------------
def write(path, content, check, stale):
    old = open(path, encoding="utf-8").read() if os.path.exists(path) else None
    if old != content:
        if check:
            stale.append(os.path.relpath(path, ROOT))
        else:
            os.makedirs(os.path.dirname(path), exist_ok=True)
            with open(path, "w", encoding="utf-8", newline="\n") as f:
                f.write(content)
            print("wrote", os.path.relpath(path, ROOT))


def cmd_check(profiles, quiet=False):
    all_e, per_w = [], {}
    ids = [p.get("board", {}).get("id") for p in profiles]
    for dup in {i for i in ids if ids.count(i) > 1}:
        all_e.append(f"board id '{dup}' used by more than one profile")
    for p in profiles:
        e, w = validate(p)
        all_e += e
        per_w[p.get("board", {}).get("id")] = w
    if not quiet:
        for w in sum(per_w.values(), []):
            print("WARNING", w)
    for e in all_e:
        print("ERROR  ", e)
    print(f"{len(profiles)} profile(s), {len(all_e)} error(s), {sum(len(w) for w in per_w.values())} warning(s)")
    return all_e, per_w


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--profiles", default=PROFILES, help="folder with *.toml profiles")
    sub = ap.add_subparsers(dest="cmd", required=True)
    sub.add_parser("check")
    g = sub.add_parser("generate")
    g.add_argument("--check", action="store_true", help="only verify generated files are current")
    sub.add_parser("list")
    n = sub.add_parser("new")
    n.add_argument("--from", dest="src", required=True)
    n.add_argument("--id", required=True)
    n.add_argument("--name", required=True)
    n.add_argument("--hw-rev", type=int)
    a = ap.parse_args()

    profiles = load_profiles(a.profiles)
    if a.cmd == "new":
        if not ID_RE.match(a.id):
            sys.exit("id must be lowercase letters, digits, underscores")
        src = [p for p in profiles if p["board"]["id"] == a.src]
        if not src:
            sys.exit(f"no profile with id {a.src}")
        dst = os.path.join(a.profiles, a.id + ".toml")
        if os.path.exists(dst):
            sys.exit(f"{dst} already exists")
        text = open(os.path.join(ROOT, src[0]["_file"]), encoding="utf-8").read()
        text = re.sub(r'(?m)^id\s*=\s*"[^"]*"', f'id       = "{a.id}"', text, count=1)
        text = re.sub(r'(?m)^name\s*=\s*"[^"]*"', f'name     = "{a.name}"', text, count=1)
        if a.hw_rev is not None:
            text = re.sub(r"(?m)^hw_rev\s*=\s*\d+", f"hw_rev   = {a.hw_rev}", text, count=1)
        text = re.sub(r"verified\s*=\s*true", "verified = false", text)
        with open(dst, "w", encoding="utf-8", newline="\n") as f:
            f.write(text)
        print(f"created {os.path.relpath(dst, ROOT)} - edit it, then run: python3 tools/vtlabs_board.py generate")
        return

    errors, warns = cmd_check(profiles, quiet=(a.cmd == "list"))
    if a.cmd == "list":
        for p in profiles:
            print(f"\nvtlabs:esp32:{p['board']['id']}  -  {p['board']['name']}")
            for n, d in p.get("pins", {}).items():
                print(f"   VTL_PIN_{n:<14} GPIO{d['gpio']:<3} {d.get('mode', ''):<7} {d.get('desc', '')}")
    if errors:
        sys.exit(1)
    if a.cmd == "generate":
        stale = []
        write(os.path.join(PLATFORM, "boards.txt"), gen_boards_txt(profiles), a.check, stale)
        for p in profiles:
            vdir = os.path.join(PLATFORM, "variants", p["board"]["id"])
            write(os.path.join(vdir, "pins_arduino.h"), gen_pins_arduino_h(p), a.check, stale)
            write(os.path.join(vdir, "vtlabs_board.h"), gen_vtlabs_board_h(p), a.check, stale)
            write(os.path.join(PINDOCS, p["board"]["id"] + ".md"), gen_pin_doc(p, warns[p["board"]["id"]]), a.check, stale)
        # remove generated variants/pin docs whose profile no longer exists (only files we generated)
        ids = {p["board"]["id"] for p in profiles}
        vroot = os.path.join(PLATFORM, "variants")
        for d in sorted(os.listdir(vroot)) if os.path.isdir(vroot) else []:
            hdr = os.path.join(vroot, d, "vtlabs_board.h")
            if d not in ids and os.path.exists(hdr) and "GENERATED by tools/vtlabs_board.py" in open(hdr, encoding="utf-8").read():
                doc = os.path.join(PINDOCS, d + ".md")
                if a.check:
                    stale.append(os.path.relpath(os.path.join(vroot, d), ROOT) + " (orphan)")
                else:
                    import shutil
                    shutil.rmtree(os.path.join(vroot, d))
                    if os.path.exists(doc):
                        os.remove(doc)
                    print("removed orphan", os.path.relpath(os.path.join(vroot, d), ROOT))
        if a.check and stale:
            print("Out of date (run: python3 tools/vtlabs_board.py generate):\n  " + "\n  ".join(stale))
            sys.exit(1)
        print("generated files are up to date" if a.check else "done")


if __name__ == "__main__":
    main()
