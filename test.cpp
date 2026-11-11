// SPDX-FileCopyrightText: © 2026 Adrian Johnston.
// SPDX-License-Identifier: MIT
// This file is licensed under the terms of the LICENSE.md file.

#include <array>
#include <cstddef>
#include <cstdio>
#include <initializer_list>
#include <print>
#include <string>
#include <string_view>
#include <type_traits>

#include "interop_generated.h"
#include "interop.hpp"

namespace test {

static_assert(__COUNTER__ == 0);
#define TEST_PLAN(n) static_assert(n == __COUNTER__ - 1); test::test_plan(n)
#define TEST_CASE(fn) passed &= test::test_case(__COUNTER__, #fn, fn())
#define CHECK(c) if (!(c)) return false

static inline void test_plan(std::size_t count) {
  std::print(stdout, "1..{}\n", count);
}

static inline bool test_case(std::size_t number, std::string_view name,
                             bool ok) {
  std::print(stdout, "{} {} - {}\n", ok ? "ok" : "not ok", number, name);
  return ok;
}

consteval bool check_target_list(std::interop_type t,
                     std::initializer_list<std::string_view> want) {
  auto targets = t.targets();
  if (targets.size() != want.size()) return false;
  std::size_t i = 0;
  for (std::string_view w : want) {
    if (std::string_view(targets[i++]) != w) return false;
  }
  return true;
}

bool test_thermostat_cpp(void) {
  thermostat_type t = thermostat_factory(20.0, 22.0);
  thermostat_type u = thermostat_factory(25.0, 18.0);

  CHECK(t.current_temp() == 20.0);
  CHECK(t.temp_setting() == 22.0);
  CHECK(t.furnace_on());
  CHECK(t.fan_on());
  CHECK(u.current_temp() == 25.0);
  CHECK(u.temp_setting() == 18.0);
  CHECK(!u.furnace_on());
  CHECK(u.fan_on());
  t.set_temp_setting(t.temp_setting() + 1.0);
  CHECK(t.temp_setting() == 23.0);
  return true;
}

bool test_thermostat_c(void) {
  void* t = thermostat_factory_(20.0, 22.0);
  void* u = thermostat_factory_(25.0, 18.0);

  CHECK(thermostat_type_current_temp_(t) == 20.0);
  CHECK(thermostat_type_temp_setting_(t) == 22.0);
  CHECK(thermostat_type_furnace_on_(t));
  CHECK(thermostat_type_fan_on_(t));
  CHECK(thermostat_type_current_temp_(u) == 25.0);
  CHECK(thermostat_type_temp_setting_(u) == 18.0);
  CHECK(!thermostat_type_furnace_on_(u));
  CHECK(thermostat_type_fan_on_(u));
  thermostat_type_set_temp_setting_(t, thermostat_type_temp_setting_(t) + 1.0);
  CHECK(thermostat_type_temp_setting_(t) == 23.0);
  thermostat_type_delete_(t);
  thermostat_type_delete_(u);
  return true;
}

consteval bool test_result_types() {
  CHECK((std::is_same_v<decltype(std::interop()), std::interop_type>));
  CHECK((std::is_same_v<decltype(std::interop), const std::interop_type>));
  return true;
}

struct stored_lists_named_type {
  constexpr operator std::string_view() const { return "named"; }
};

consteval std::interop_type stored_lists_from_local() {
  char local[] = "local";
  return std::interop(std::string_view(local));
}

constexpr const char* stored_lists_pointer = "ptr";

constexpr auto stored_lists_escaped = stored_lists_from_local();
constexpr auto stored_lists_from_types = std::interop(
    std::string("ab") + "c", stored_lists_named_type{}, stored_lists_pointer);

consteval bool test_stored_list() {
  CHECK(check_target_list(stored_lists_escaped, {"local"}));
  CHECK(check_target_list(stored_lists_from_types, {"abc", "named", "ptr"}));
  return true;
}

template<std::size_t N>
consteval std::array<char, N> computed_views_make_text() {
  std::array<char, N> text{};
  for (std::size_t i = 0; i < N; ++i) {
    text[i] = static_cast<char>('a' + i % 26);
  }
  return text;
}

constexpr auto computed_views_computed = computed_views_make_text<4>();

consteval bool test_computed_view() {
  auto whole =
      std::interop(std::string_view(computed_views_computed.data(), 4));
  CHECK(check_target_list(whole, {"abcd"}));
  return true;
}

consteval bool test_literal_null() {
  auto literal_nul = std::interop("a\0b");
  CHECK(std::string_view(literal_nul.targets()[0], 3) ==
        std::string_view("a\0b", 3));
  return true;
}

consteval bool test_call_chain() {
  CHECK(check_target_list(std::interop("a")("b")("c"), {"a", "b", "c"}));
  return true;
}

}

int main() {
  std::print(stdout, "# cc: {}\n", __VERSION__);
  std::print(stdout, "# testing: interop.hpp interop_generated.h"
    " interop_generated.cpp\n");
  bool passed = true;
  TEST_CASE(test::test_thermostat_cpp);
  TEST_CASE(test::test_thermostat_c);
  TEST_CASE(test::test_result_types);
  TEST_CASE(test::test_stored_list);
  TEST_CASE(test::test_computed_view);
  TEST_CASE(test::test_literal_null);
  TEST_CASE(test::test_call_chain);
  TEST_PLAN(7);
  return !passed;
}
