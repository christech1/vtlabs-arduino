#!/usr/bin/env python3
"""Unit tests for the pin-map checker in tools/vtlabs_board.py.

    python3 -m unittest discover -s tests -p "test_*.py"
"""
import copy
import os
import sys
import unittest

sys.path.insert(0, os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "tools"))
import vtlabs_board as vb  # noqa: E402

BASE = vb.load_profiles()[0]


def errors_for(mutate):
    p = copy.deepcopy(BASE)
    mutate(p)
    e, _ = vb.validate(p)
    return " | ".join(e)


class PinChecks(unittest.TestCase):
    def test_shipped_profile_is_clean(self):
        e, _ = vb.validate(copy.deepcopy(BASE))
        self.assertEqual(e, [])

    def test_duplicate_gpio(self):
        self.assertIn("used twice", errors_for(lambda p: p["pins"]["BUZZER"].update(gpio=25)))

    def test_flash_pin(self):
        self.assertIn("SPI flash", errors_for(lambda p: p["pins"]["BUZZER"].update(gpio=7)))

    def test_output_on_input_only(self):
        self.assertIn("input-only", errors_for(lambda p: p["pins"]["RELAY"].update(gpio=34)))

    def test_internal_pullup_on_input_only(self):
        self.assertIn("no internal pull", errors_for(lambda p: p["pins"]["BTN1"].update(pull="internal")))

    def test_analog_without_adc(self):
        self.assertIn("has no ADC", errors_for(lambda p: p["pins"]["POT"].update(gpio=23)))

    def test_gpio12_pullup_boot_failure(self):
        def m(p):
            p["pins"]["BTN1"].update(gpio=12, pull="external")
        self.assertIn("GPIO12", errors_for(m))

    def test_nonexistent_gpio(self):
        self.assertIn("does not exist", errors_for(lambda p: p["pins"]["BUZZER"].update(gpio=24)))

    def test_missing_curriculum_name(self):
        self.assertIn("missing PIR", errors_for(lambda p: p["pins"].pop("PIR")))

    def test_option_pin_conflicts_with_base(self):
        def m(p):
            p["option"][1]["choice"][1]["pins"]["MAX7219_CS"]["gpio"] = 4  # RELAY
        self.assertIn("used twice", errors_for(m))

    def test_partition_needs_more_flash(self):
        self.assertIn("needs 16MB", errors_for(lambda p: p["build"]["partitions"].append("fatflash")))

    def test_user_pin_on_free_gpio_is_ok(self):
        def m(p):
            p["pins"]["SERVO"] = {"gpio": 12, "mode": "pwm", "desc": "servo"}
        self.assertEqual(errors_for(m), "")

    def test_bad_name(self):
        self.assertIn("UPPER_CASE", errors_for(lambda p: p["pins"].update({"myPin": {"gpio": 12, "mode": "out"}})))


class Generation(unittest.TestCase):
    def test_header_contains_override_guards(self):
        h = vb.gen_vtlabs_board_h(copy.deepcopy(BASE))
        self.assertIn("#ifndef VTL_PIN_LED_RED", h)
        self.assertIn("#define VTL_OPT_KEYPAD_NONE", h)

    def test_boards_txt_menus(self):
        b = vb.gen_boards_txt([copy.deepcopy(BASE)])
        self.assertIn("vtlabs_iot_esp32_r1.build.core=esp32:esp32", b)
        self.assertIn("menu.Keypad.pcf8574_4x4.build.vtl_opt_keypad=-DVTL_OPT_KEYPAD_PCF8574_4X4", b)
        self.assertIn("menu.LedMatrix=LED Matrix (add-on)", b)


if __name__ == "__main__":
    unittest.main()
