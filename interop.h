// SPDX-FileCopyrightText: © 2026 Adrian Johnston.
// SPDX-License-Identifier: MIT
// This file is licensed under the terms of the LICENSE.md file.

#pragma once

#include <array>
#include <concepts>
#include <cstddef>
#include <meta>
#include <span>
#include <string_view>

namespace std {
  struct interop_type {
    template<convertible_to<string_view>... S>
    consteval interop_type operator()(S... s) const {       // exposition only
      auto stored_ = define_static_array(array<const char*, sizeof...(S)>{
          define_static_string(string_view(s))...});
      return {stored_.data(), stored_.size()};
    }
    constexpr span<const char* const> targets(void) const { // exposition only
      return {list_, size_};
    }
    const char* const* list_ = nullptr;                     // exposition only
    size_t size_ = 0;                                       // exposition only
  };
  inline constexpr interop_type interop{};
}

inline constexpr auto script = std::interop("ECMAScript", "Python");

class [[=std::interop]] thermostat_type {
public:
  [[=std::interop]] ~thermostat_type(void) = default;

  [[=std::interop]] double current_temp(void) const { return current_; }
  [[=std::interop]] double temp_setting(void) const { return setting_; }

  [[=script]] bool furnace_on(void) const { return setting_ > current_; }
  [[=script]] bool fan_on(void) const { return true; }

  [[=std::interop("Rust")]] void set_temp_setting(double t_) {
    setting_ = t_;
  }

private:
  friend thermostat_type thermostat_factory(double current, double setting);
  thermostat_type(double current, double setting)
      : current_(current), setting_(setting) {}

  double current_;
  double setting_;
};

[[=std::interop]] thermostat_type thermostat_factory(double current,
                                                     double setting) {
  return thermostat_type{current, setting};
}
