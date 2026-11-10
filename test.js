// SPDX-FileCopyrightText: © 2026 Adrian Johnston.
// SPDX-License-Identifier: MIT
// This file is licensed under the terms of the LICENSE.md file.

const koffi = require("koffi");
const interop = require("interop_generated");

console.log(`# Node ${process.version}, koffi ${koffi.version}`);
console.log(`# binding: ${require.resolve("interop_generated")}`);

const t = interop.thermostat_factory(20, 22);
const u = interop.thermostat_factory(25, 18);

const prototype = Object.getPrototypeOf(t);
const bindings = [
  ...Object.getOwnPropertyNames(prototype)
    .filter((name) => name !== "constructor")
    .map((name) => [name, prototype[name]]),
  ...Object.entries(interop),
];
for (const [name, binding] of bindings) {
  console.log(`# binds: ${name}${/\(.*?\)/.exec(binding)[0]}`);
}

const checks = [
  ["t.current_temp()", t.current_temp(), 20],
  ["t.temp_setting()", t.temp_setting(), 22],
  ["t.furnace_on()", t.furnace_on(), true],
  ["t.fan_on()", t.fan_on(), true],
  ["u.current_temp()", u.current_temp(), 25],
  ["u.temp_setting()", u.temp_setting(), 18],
  ["u.furnace_on()", u.furnace_on(), false],
  ["u.fan_on()", u.fan_on(), true],
];

let failed = false;
for (const [index, [name, found, wanted]] of checks.entries()) {
  const passed = found === wanted;
  failed ||= !passed;
  console.log(`${passed ? "ok" : "not ok"} ${index + 1} - ${name}`);
  console.log("  ---");
  console.log(`  type: ${typeof found}`);
  console.log(`  found: ${found}`);
  console.log(`  wanted: ${wanted}`);
  console.log("  ...");
}
console.log(`1..${checks.length}`);
process.exitCode = failed ? 1 : 0;
