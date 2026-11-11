// SPDX-FileCopyrightText: © 2026 Adrian Johnston.
// SPDX-License-Identifier: MIT
// This file is licensed under the terms of the LICENSE.md file.

#pragma once

#include <concepts>
#include <cstddef>
#include <meta>
#include <span>
#include <string_view>
#include <type_traits>
#include <vector>

namespace std {
  struct interop_type {
    template<convertible_to<string_view>... S>
    consteval interop_type operator()(const S&... s) const {
      vector<const char*> list(list_, list_ + size_);
      (list.push_back(define_static_string(whole_(s))), ...);
      auto kept = define_static_array(list);
      return {kept.data(), kept.size()};
    }
    constexpr span<const char* const> targets(void) const {
      return {list_, size_};
    }
    const char* const* list_;                         // exposition only
    size_t size_;                                     // exposition only
    template<class S>
    static consteval string_view whole_(const S& s) { // exposition only
      if constexpr (is_array_v<S>) return {s, extent_v<S> - 1};
      else return s;
    }
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
