// SPDX-FileCopyrightText: © 2026 Adrian Johnston.
// SPDX-License-Identifier: MIT
// This file is licensed under the terms of the LICENSE.md file.

const koffi = require("koffi");

const lib = koffi.load("libinterop.so");

koffi.alias("thermostat_type", "void*");

const thermostat_type_delete_ = lib.func(
  "void thermostat_type_delete_(void*)",
);

const thermostat_type_current_temp_ = lib.func(
  "double thermostat_type_current_temp_(void*)",
);

const thermostat_type_temp_setting_ = lib.func(
  "double thermostat_type_temp_setting_(void*)",
);

const thermostat_type_furnace_on_ = lib.func(
  "bool thermostat_type_furnace_on_(void*)",
);

const thermostat_type_fan_on_ = lib.func(
  "bool thermostat_type_fan_on_(void*)",
);

const thermostat_type = {
  current_temp() {
    return thermostat_type_current_temp_(this.handle);
  },

  temp_setting() {
    return thermostat_type_temp_setting_(this.handle);
  },

  furnace_on() {
    return thermostat_type_furnace_on_(this.handle);
  },

  fan_on() {
    return thermostat_type_fan_on_(this.handle);
  },

  [Symbol.dispose]() {
    thermostat_type_delete_(this.handle);
  },
};

const thermostat_factory_ = lib.func(
  "thermostat_type thermostat_factory_(double, double)",
);

exports.thermostat_factory = (current, setting) => {
  return Object.create(thermostat_type, {
    handle: { value: thermostat_factory_(current, setting) },
  });
};
