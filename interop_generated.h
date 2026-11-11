// SPDX-FileCopyrightText: © 2026 Adrian Johnston.
// SPDX-License-Identifier: MIT
// This file is licensed under the terms of the LICENSE.md file.

#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void thermostat_type_delete_(void* self);
double thermostat_type_current_temp_(void* self);
double thermostat_type_temp_setting_(void* self);
bool thermostat_type_furnace_on_(void* self);
bool thermostat_type_fan_on_(void* self);
void thermostat_type_set_temp_setting_(void* self, double t_);

void* thermostat_factory_(double current, double setting);

#ifdef __cplusplus
}
#endif
