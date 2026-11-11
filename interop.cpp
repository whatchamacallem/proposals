// SPDX-FileCopyrightText: © 2026 Adrian Johnston.
// SPDX-License-Identifier: MIT
// This file is licensed under the terms of the LICENSE.md file.

#include "interop.hpp"

#include <algorithm>
#include <format>
#include <fstream>
#include <meta>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

namespace tool {
  constexpr auto ctx = std::meta::access_context::current();

  constexpr std::string_view license =
      "{0} SPDX-FileCopyrightText: © 2026 Adrian Johnston.\n"
      "{0} SPDX-License-Identifier: MIT\n"
      "{0} This file is licensed under the terms of the LICENSE.md file.\n\n";

  struct c_header {
    static constexpr std::string_view language = "";
    static constexpr const char* file = "interop_generated.h";
    static constexpr std::string_view comment = "//";
    static constexpr std::string_view file_body =
        "#pragma once\n\n"
        "#include <stdbool.h>\n\n"
        "#ifdef __cplusplus\n"
        "extern \"C\" {{\n"
        "#endif\n"
        "{0}{1}\n"
        "#ifdef __cplusplus\n"
        "}}\n"
        "#endif\n";
    static constexpr std::string_view foreign_declaration =
        "{2} {3}_{0}_({5});\n";
    static constexpr std::string_view class_body = "\n{1}";
    static constexpr std::string_view destructor = "";
    static constexpr std::string_view method = "";
    static constexpr std::string_view void_method = "";
    static constexpr std::string_view function = "\nvoid* {0}_({5});\n";
    static constexpr std::string_view parameter = "";
    static constexpr std::string_view foreign = "{1} {0}";
    static constexpr std::string_view handle = "";
    static constexpr std::string_view receiver = "void* self";

    static consteval std::string_view self(std::meta::info) {
      return "";
    }
    static consteval std::string_view type(std::meta::info t) {
      return std::meta::display_string_of(t);
    }
    static consteval std::string_view hint(std::meta::info t) {
      return type(t);
    }
  };

  struct c_source {
    static constexpr std::string_view language = "";
    static constexpr const char* file = "interop_generated.cpp";
    static constexpr std::string_view comment = "//";
    static constexpr std::string_view file_body =
        "#include \"interop.hpp\"\n\n"
        "extern \"C\" {{\n"
        "{0}{1}}}\n";
    static constexpr std::string_view foreign_declaration = "";
    static constexpr std::string_view class_body = "\n{2}{3}";
    static constexpr std::string_view destructor =
        "\nvoid {3}_{0}_({5}) noexcept {{\n"
        "  delete static_cast<{3}*>(self);\n"
        "}}\n";
    static constexpr std::string_view method =
        "{2} {3}_{0}_({5}) noexcept {{\n"
        "  return static_cast<{3}*>(self)->{0}({4});\n"
        "}}\n";
    static constexpr std::string_view void_method =
        "void {3}_{0}_({5}) noexcept {{\n"
        "  static_cast<{3}*>(self)->{0}({4});\n"
        "}}\n";
    static constexpr std::string_view function =
        "\nvoid* {0}_({5}) noexcept {{\n"
        "  return new auto(::{0}({4}));\n"
        "}}\n";
    static constexpr std::string_view parameter = "";
    static constexpr std::string_view foreign = "{1} {0}";
    static constexpr std::string_view handle = "";
    static constexpr std::string_view receiver = "void* self";

    static consteval std::string_view self(std::meta::info) {
      return "";
    }
    static consteval std::string_view type(std::meta::info t) {
      return std::meta::display_string_of(t);
    }
    static consteval std::string_view hint(std::meta::info t) {
      return type(t);
    }
  };

  struct ecmascript {
    static constexpr std::string_view language = "ECMAScript";
    static constexpr const char* file = "interop_generated.js";
    static constexpr std::string_view comment = "//";
    static constexpr std::string_view file_body =
        "const koffi = require(\"koffi\");\n"
        "\nconst lib = koffi.load(\"libinterop.so\");\n"
        "{0}{1}";
    static constexpr std::string_view foreign_declaration =
        "\nconst {3}_{0}_ = lib.func(\n"
        "  \"{2} {3}_{0}_({5})\",\n"
        ");\n";
    static constexpr std::string_view class_body =
        "\nkoffi.alias(\"{0}\", \"void*\");\n"
        "{1}\n"
        "const {0} = {{\n"
        "{2}{3}}};\n";
    static constexpr std::string_view destructor =
        "\n  [Symbol.dispose]() {{\n"
        "    {3}_{0}_({4});\n"
        "  }},\n";
    static constexpr std::string_view method =
        "  {0}({1}) {{\n"
        "    return {3}_{0}_({4});\n"
        "  }},\n";
    static constexpr std::string_view void_method = method;
    static constexpr std::string_view function =
        "\nconst {0}_ = lib.func(\n"
        "  \"{2} {0}_({5})\",\n"
        ");\n\n"
        "exports.{0} = ({1}) => {{\n"
        "  return Object.create({2}, {{\n"
        "    handle: {{ value: {0}_({4}) }},\n"
        "  }});\n"
        "}};\n";
    static constexpr std::string_view parameter = "{0}";
    static constexpr std::string_view foreign = "{1}";
    static constexpr std::string_view handle = "this.handle";
    static constexpr std::string_view receiver = "void*";

    static consteval std::string_view self(std::meta::info) {
      return "";
    }
    static consteval std::string_view type(std::meta::info t) {
      return std::meta::display_string_of(t);
    }
    static consteval std::string_view hint(std::meta::info t) {
      return type(t);
    }
  };

  struct python {
    static constexpr std::string_view language = "Python";
    static constexpr const char* file = "interop_generated.py";
    static constexpr std::string_view comment = "#";
    static constexpr std::string_view file_body =
        "import ctypes\n\n"
        "lib = ctypes.CDLL(\"libinterop.so\")\n"
        "{0}{1}";
    static constexpr std::string_view foreign_declaration =
        "\nlib.{3}_{0}_.restype = {2}\n"
        "lib.{3}_{0}_.argtypes = [{5}]\n";
    static constexpr std::string_view class_body =
        "{1}\n\n"
        "class {0}(ctypes.c_void_p):\n"
        "{2}{3}\n";
    static constexpr std::string_view destructor =
        "\n    def __del__({1}) -> {6}:\n"
        "        lib.{3}_{0}_({4})\n";
    static constexpr std::string_view method =
        "    def {0}({1}) -> {6}:\n"
        "        return lib.{3}_{0}_({4})\n";
    static constexpr std::string_view void_method = method;
    static constexpr std::string_view function =
        "\nlib.{0}_.restype = {2}\n"
        "lib.{0}_.argtypes = [{5}]\n\n\n"
        "def {0}({1}) -> {6}:\n"
        "    return lib.{0}_({4})\n";
    static constexpr std::string_view parameter = "{0}: {2}";
    static constexpr std::string_view foreign = "{1}";
    static constexpr std::string_view handle = "self";
    static constexpr std::string_view receiver = "ctypes.c_void_p";

    static consteval std::string_view self(std::meta::info) {
      return "self";
    }
    static consteval std::string_view type(std::meta::info t) {
      return t == ^^void     ? "None"
             : t == ^^double ? "ctypes.c_double"
             : t == ^^bool   ? "ctypes.c_bool"
                             : std::meta::display_string_of(t);
    }
    static consteval std::string_view hint(std::meta::info t) {
      return t == ^^void     ? "None"
             : t == ^^double ? "float"
             : t == ^^bool   ? "bool"
                             : std::meta::display_string_of(t);
    }
  };

  struct rust {
    static constexpr std::string_view language = "Rust";
    static constexpr const char* file = "interop_generated.rs";
    static constexpr std::string_view comment = "//";
    static constexpr std::string_view file_body =
        "use std::ffi::c_void;\n"
        "{0}{1}";
    static constexpr std::string_view foreign_declaration = "";
    static constexpr std::string_view class_body =
        "\n#[allow(non_camel_case_types)]\n"
        "#[repr(transparent)]\n"
        "pub struct {0}(*mut c_void);\n"
        "\nimpl {0} {{\n"
        "{2}}}\n"
        "{3}";
    static constexpr std::string_view destructor =
        "\nimpl Drop for {3} {{\n"
        "    fn drop({1}) {{\n"
        "        unsafe extern \"C\" {{\n"
        "            safe fn {3}_{0}_({5});\n"
        "        }}\n"
        "        {3}_{0}_({4});\n"
        "    }}\n"
        "}}\n";
    static constexpr std::string_view method =
        "    #[must_use]\n"
        "    pub fn {0}({1}) -> {2} {{\n"
        "        unsafe extern \"C\" {{\n"
        "            safe fn {3}_{0}_({5}) -> {2};\n"
        "        }}\n"
        "        {3}_{0}_({4})\n"
        "    }}\n";
    static constexpr std::string_view void_method =
        "    pub fn {0}({1}) {{\n"
        "        unsafe extern \"C\" {{\n"
        "            safe fn {3}_{0}_({5});\n"
        "        }}\n"
        "        {3}_{0}_({4});\n"
        "    }}\n";
    static constexpr std::string_view function =
        "\n#[must_use]\n"
        "pub fn {0}({1}) -> {2} {{\n"
        "    unsafe extern \"C\" {{\n"
        "        safe fn {0}_({5}) -> {2};\n"
        "    }}\n"
        "    {0}_({4})\n"
        "}}\n";
    static constexpr std::string_view parameter = "{0}: {1}";
    static constexpr std::string_view foreign = "_: {1}";
    static constexpr std::string_view handle = "self.0";
    static constexpr std::string_view receiver = "_: *mut c_void";

    static consteval std::string_view self(std::meta::info fn) {
      return std::meta::is_const(fn) ? "&self" : "&mut self";
    }
    static consteval std::string_view type(std::meta::info t) {
      return t == ^^double ? "f64" : std::meta::display_string_of(t);
    }
    static consteval std::string_view hint(std::meta::info t) {
      return type(t);
    }
  };

  consteval std::optional<std::interop_type> annotation_of(
      std::meta::info decl) {
    auto found =
        std::meta::annotations_of_with_type(decl, ^^std::interop_type);
    if (found.empty()) return std::nullopt;
    return std::meta::extract<std::interop_type>(found[0]);
  }

  consteval bool binds_to(std::meta::info fn, std::string_view language) {
    auto annotation = annotation_of(fn);
    if (!annotation) return false;
    if (annotation->targets().empty() && std::meta::is_class_member(fn)) {
      annotation =
          annotation_of(std::meta::parent_of(fn)).value_or(*annotation);
    }
    return language.empty() || annotation->targets().empty() ||
           std::ranges::contains(annotation->targets(), language);
  }

  consteval std::vector<std::meta::info> bound_functions(
      std::meta::info scope, std::string_view language) {
    return std::meta::members_of(scope, ctx) |
           std::views::filter([=](std::meta::info m) {
             return std::meta::is_function(m) && binds_to(m, language);
           }) |
           std::ranges::to<std::vector>();
  }

  consteval std::vector<std::meta::info> bound_classes(
      std::string_view language) {
    return std::meta::members_of(^^::, ctx) |
           std::views::filter([=](std::meta::info d) {
             return std::meta::is_type(d) && std::meta::is_class_type(d) &&
                    std::meta::is_complete_type(d) &&
                    !bound_functions(d, language).empty();
           }) |
           std::ranges::to<std::vector>();
  }

  consteval std::string_view callable_name(std::meta::info fn) {
    return std::meta::is_destructor(fn) ? "delete"
                                        : std::meta::identifier_of(fn);
  }

  consteval std::meta::info result_type(std::meta::info fn) {
    return std::meta::is_destructor(fn) ? ^^void
                                        : std::meta::return_type_of(fn);
  }

  consteval std::string_view scope_name(std::meta::info fn) {
    return std::meta::is_class_member(fn)
               ? std::meta::identifier_of(std::meta::parent_of(fn))
               : "";
  }

  std::string comma_list(const std::vector<std::string>& items) {
    auto filled = [](const std::string& item) { return !item.empty(); };
    return std::format("{:n:}", items | std::views::filter(filled));
  }

  template<class L, std::meta::info Fn, const std::string_view& Format>
  std::string format_callable() {
    std::vector<std::string> parameters, arguments, foreign;
    if constexpr (std::meta::is_class_member(Fn)) {
      parameters.emplace_back(L::self(Fn));
      arguments.emplace_back(L::handle);
      foreign.emplace_back(L::receiver);
    }
    template for (constexpr std::meta::info p :
        std::define_static_array(std::meta::parameters_of(Fn))) {
      constexpr std::string_view name = std::meta::identifier_of(p);
      constexpr std::meta::info type = std::meta::type_of(p);
      parameters.push_back(
          std::format(L::parameter, name, L::type(type), L::hint(type)));
      arguments.emplace_back(name);
      foreign.push_back(std::format(L::foreign, name, L::type(type)));
    }
    constexpr std::meta::info result = result_type(Fn);
    return std::format(Format, callable_name(Fn), comma_list(parameters),
                       L::type(result), scope_name(Fn), comma_list(arguments),
                       comma_list(foreign), L::hint(result));
  }

  template<class L>
  void generate_bindings() {
    std::string classes;
    template for (constexpr std::meta::info cls :
        std::define_static_array(bound_classes(L::language))) {
      std::string declarations, destructor;
      std::vector<std::string> methods;
      template for (constexpr std::meta::info fn :
          std::define_static_array(bound_functions(cls, L::language))) {
        declarations += format_callable<L, fn, L::foreign_declaration>();
        if constexpr (std::meta::is_destructor(fn)) {
          destructor = format_callable<L, fn, L::destructor>();
        } else {
          constexpr auto& layout =
              result_type(fn) == ^^void ? L::void_method : L::method;
          methods.push_back(format_callable<L, fn, layout>());
        }
      }
      classes += std::format(
          L::class_body, std::meta::identifier_of(cls), declarations,
          std::format("{:s}", methods | std::views::join_with('\n')),
          destructor);
    }
    std::string functions;
    template for (constexpr std::meta::info fn :
        std::define_static_array(bound_functions(^^::, L::language))) {
      functions += format_callable<L, fn, L::function>();
    }
    std::ofstream out(L::file);
    std::print(out, license, L::comment);
    std::print(out, L::file_body, classes, functions);
  }
}

int main() {
  tool::generate_bindings<tool::c_header>();
  tool::generate_bindings<tool::c_source>();
  tool::generate_bindings<tool::ecmascript>();
  tool::generate_bindings<tool::python>();
  tool::generate_bindings<tool::rust>();
}
