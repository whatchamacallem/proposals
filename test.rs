// SPDX-FileCopyrightText: © 2026 Adrian Johnston.
// SPDX-License-Identifier: MIT
// This file is licensed under the terms of the LICENSE.md file.

include!(env!("INTEROP_RS"));

fn main() -> std::process::ExitCode {
    println!("# {}", env!("RUSTC_VERSION"));
    println!("# binding: {}", env!("INTEROP_RS"));
    for line in include_str!(env!("INTEROP_RS")).lines() {
        if let Some(signature) = line.trim().strip_prefix("pub fn ") {
            println!("# binds: {}", signature.trim_end_matches(" {"));
        }
    }

    let mut t = thermostat_factory(20.0, 22.0);
    let u = thermostat_factory(25.0, 18.0);
    let mut checks = vec![
        ("t.current_temp()", t.current_temp(), 20.0),
        ("t.temp_setting()", t.temp_setting(), 22.0),
        ("u.current_temp()", u.current_temp(), 25.0),
        ("u.temp_setting()", u.temp_setting(), 18.0),
    ];
    t.set_temp_setting(t.temp_setting() + 1.0);
    checks.push(("t.temp_setting() raised", t.temp_setting(), 23.0));

    let mut failed = false;
    for (index, (name, found, wanted)) in checks.iter().enumerate() {
        let passed = found == wanted;
        failed |= !passed;
        let status = if passed { "ok" } else { "not ok" };
        println!("{status} {} - {name}", index + 1);
        println!("  ---");
        println!("  type: {}", std::any::type_name_of_val(found));
        println!("  found: {found:?}");
        println!("  wanted: {wanted:?}");
        println!("  ...");
    }
    println!("1..{}", checks.len());
    std::process::ExitCode::from(u8::from(failed))
}
