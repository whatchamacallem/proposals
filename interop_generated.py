# SPDX-FileCopyrightText: © 2026 Adrian Johnston.
# SPDX-License-Identifier: MIT
# This file is licensed under the terms of the LICENSE.md file.

import ctypes

lib = ctypes.CDLL("libinterop.so")

lib.thermostat_type_delete_.restype = None
lib.thermostat_type_delete_.argtypes = [ctypes.c_void_p]

lib.thermostat_type_current_temp_.restype = ctypes.c_double
lib.thermostat_type_current_temp_.argtypes = [ctypes.c_void_p]

lib.thermostat_type_temp_setting_.restype = ctypes.c_double
lib.thermostat_type_temp_setting_.argtypes = [ctypes.c_void_p]

lib.thermostat_type_furnace_on_.restype = ctypes.c_bool
lib.thermostat_type_furnace_on_.argtypes = [ctypes.c_void_p]

lib.thermostat_type_fan_on_.restype = ctypes.c_bool
lib.thermostat_type_fan_on_.argtypes = [ctypes.c_void_p]


class thermostat_type(ctypes.c_void_p):
    def current_temp(self) -> float:
        return lib.thermostat_type_current_temp_(self)

    def temp_setting(self) -> float:
        return lib.thermostat_type_temp_setting_(self)

    def furnace_on(self) -> bool:
        return lib.thermostat_type_furnace_on_(self)

    def fan_on(self) -> bool:
        return lib.thermostat_type_fan_on_(self)

    def __del__(self) -> None:
        lib.thermostat_type_delete_(self)


lib.thermostat_factory_.restype = thermostat_type
lib.thermostat_factory_.argtypes = [ctypes.c_double, ctypes.c_double]


def thermostat_factory(current: float, setting: float) -> thermostat_type:
    return lib.thermostat_factory_(current, setting)
