# SPDX-FileCopyrightText: © 2026 Adrian Johnston.
# SPDX-License-Identifier: MIT
# This file is licensed under the terms of the LICENSE.md file.

import inspect
import json
import platform
import sys

import interop_generated as interop

print(f"# python: {platform.python_implementation()} {platform.python_version()}")
print(f"# binding: {interop.__file__}")

t = interop.thermostat_factory(20, 22)
u = interop.thermostat_factory(25, 18)

checks = [
    ("t.current_temp()", t.current_temp(), 20.0),
    ("t.temp_setting()", t.temp_setting(), 22.0),
    ("t.furnace_on()", t.furnace_on(), True),
    ("t.fan_on()", t.fan_on(), True),
    ("u.current_temp()", u.current_temp(), 25.0),
    ("u.temp_setting()", u.temp_setting(), 18.0),
    ("u.furnace_on()", u.furnace_on(), False),
    ("u.fan_on()", u.fan_on(), True),
]

failed = False
for number, (name, found, wanted) in enumerate(checks, 1):
    passed = found == wanted
    failed = failed or not passed
    print(f"{'ok' if passed else 'not ok'} {number} - {name}")
    print("  ---")
    print(f"  type: {type(found).__name__}")
    print(f"  found: {json.dumps(found)}")
    print(f"  wanted: {json.dumps(wanted)}")
    print("  ...")
print(f"1..{len(checks)}")
sys.exit(failed)
