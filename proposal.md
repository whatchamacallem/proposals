---
title: "[[=std::interop]] An Annotation for Interoperation with Targets"
document: D0000R0
date: unassigned
audience:
  - SG7
  - LEWG
author:
  - name: Adrian Johnston
    email: <ajohnston54637@gmail.com>
toc: false
---

# Motivation

To propose a standard annotation, `[[=std::interop]]`, marking a declaration
for exposure to a foreign execution environment, with an optional target list.
It has no effect on the program and no target list semantics are to be
specified. As context, [@P3394R4] supplies the mechanism but no vocabulary, so
each library will define its own `exposed_t`. By adding `[[=std::interop]]`
different libraries and tools should not need to define their own `exposed_t`.
This attribute is proposed with the intention of it being used as the basis
for non-proliferation treaties around interoperability annotation. Although,
that is hard to control.

# Proposal

This proposal adds a class `interop_type` and a constant `interop` of that type
to a new header, `<interop>`. The `interop` constant and the result of calling
it with `operator()` are required to be valid annotation operands under
[@P3394R4]. The call operator accepts any number of arguments convertible to
`string_view` and returns an `interop_type` holding copies of them, in static
storage and in call order. `targets()` returns that list, empty for `interop`
itself. Both are usable in constant expressions. Nothing else about the type
is specified; the exposition-only members show one way to meet this.

```cpp
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
```

The annotation may appear wherever [@P3394R4] allows one. `[[=std::interop]]`
marks a declaration for exposure without naming a target, and
`[[=std::interop("Rust")]]` names its targets. A target list can also be
built once and reused, as `script` is below. The library attaches no meaning
to the strings, to their order, or to an empty list; a tool reading the
annotation through `annotations_of` decides. Annotating a class says nothing
about its members, so each member to be exposed carries its own annotation,
including the destructor.

```cpp
inline constexpr auto script = std::interop("ECMAScript", "Python");

class [[=std::interop]] thermostat_type {
public:
  [[=std::interop]] ~thermostat_type(void) = default;

  [[=std::interop]] double current_temp(void) const;
  [[=std::interop]] double temp_setting(void) const;

  [[=script]] bool furnace_on(void) const;
  [[=script]] bool fan_on(void) const;

  [[=std::interop("Rust")]] void set_temp_setting(double t_);

private:
  // ...
};

[[=std::interop]] thermostat_type thermostat_factory(double current,
                                                     double setting);
```

# Design Decisions

- Naming languages in the standard library is recognized as unmaintainable.
Other standards are welcome to make recommendations for their own naming
schemes.
- `interop_type` is implementable with `define_static_string` and
`define_static_array` from [@P3491R3].

**Future work.** [@P3394R4] permits annotations on namespaces, but the meaning
of `std::interop` on a namespace for its members is left open. Discussion is
needed with independent interoperability project maintainers once this
proposal is concrete.
