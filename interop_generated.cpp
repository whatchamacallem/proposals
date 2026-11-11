// SPDX-FileCopyrightText: © 2026 Adrian Johnston.
// SPDX-License-Identifier: MIT
// This file is licensed under the terms of the LICENSE.md file.

#include "interop.hpp"

extern "C" {

double thermostat_type_current_temp_(void* self) noexcept {
  return static_cast<thermostat_type*>(self)->current_temp();
}

double thermostat_type_temp_setting_(void* self) noexcept {
  return static_cast<thermostat_type*>(self)->temp_setting();
}

bool thermostat_type_furnace_on_(void* self) noexcept {
  return static_cast<thermostat_type*>(self)->furnace_on();
}

bool thermostat_type_fan_on_(void* self) noexcept {
  return static_cast<thermostat_type*>(self)->fan_on();
}

void thermostat_type_set_temp_setting_(void* self, double t_) noexcept {
  static_cast<thermostat_type*>(self)->set_temp_setting(t_);
}

void thermostat_type_delete_(void* self) noexcept {
  delete static_cast<thermostat_type*>(self);
}

void* thermostat_factory_(double current, double setting) noexcept {
  return new auto(::thermostat_factory(current, setting));
}
}
