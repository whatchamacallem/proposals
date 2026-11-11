// SPDX-FileCopyrightText: © 2026 Adrian Johnston.
// SPDX-License-Identifier: MIT
// This file is licensed under the terms of the LICENSE.md file.

use std::ffi::c_void;

#[allow(non_camel_case_types)]
#[repr(transparent)]
pub struct thermostat_type(*mut c_void);

impl thermostat_type {
    #[must_use]
    pub fn current_temp(&self) -> f64 {
        unsafe extern "C" {
            safe fn thermostat_type_current_temp_(_: *mut c_void) -> f64;
        }
        thermostat_type_current_temp_(self.0)
    }

    #[must_use]
    pub fn temp_setting(&self) -> f64 {
        unsafe extern "C" {
            safe fn thermostat_type_temp_setting_(_: *mut c_void) -> f64;
        }
        thermostat_type_temp_setting_(self.0)
    }

    pub fn set_temp_setting(&mut self, t_: f64) {
        unsafe extern "C" {
            safe fn thermostat_type_set_temp_setting_(_: *mut c_void, _: f64);
        }
        thermostat_type_set_temp_setting_(self.0, t_);
    }
}

impl Drop for thermostat_type {
    fn drop(&mut self) {
        unsafe extern "C" {
            safe fn thermostat_type_delete_(_: *mut c_void);
        }
        thermostat_type_delete_(self.0);
    }
}

#[must_use]
pub fn thermostat_factory(current: f64, setting: f64) -> thermostat_type {
    unsafe extern "C" {
        safe fn thermostat_factory_(_: f64, _: f64) -> thermostat_type;
    }
    thermostat_factory_(current, setting)
}
