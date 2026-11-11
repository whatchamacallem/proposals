#!/usr/bin/env bash
# SPDX-FileCopyrightText: © 2026 Adrian Johnston.
# SPDX-License-Identifier: MIT
# This file is licensed under the terms of the LICENSE.md file.

set -euo pipefail

open_stream() {
  [[ ! -s $tap ]] || return 0
  echo "TAP version 14"
  echo "# SPDX-FileCopyrightText: © 2026 Adrian Johnston."
  echo "# SPDX-License-Identifier: MIT"
  echo "# This file is licensed under the terms of the LICENSE.md file."
}

fail_build() {
  echo "Bail out! $*" >&2
  exit 1
}

start_clock() {
  started=$(date +%s.%N)
}

locate_root() {
  root=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
  script=$root/scripts/build.sh
}

parse_arguments() {
  proposal=$root/proposal.md
  if [[ $# -gt 0 && $1 == *.md ]]; then
    proposal=$1
    shift
  fi

  formats=("$@")
  [[ ${#formats[@]} -gt 0 ]] || formats=(pdf html latex bindings)
}

locate_proposal() {
  [[ -f $proposal ]] || proposal=$root/$proposal
  [[ -f $proposal ]] || fail_build "no such proposal: ${proposal#"$root"/}"

  paper_dir=$(cd "$(dirname "$proposal")" && pwd)
  paper=$(basename "$proposal" .md)
  out=$paper_dir/staging

  [[ $root$paper_dir$paper != *[[:space:]]* ]] ||
    fail_build "the framework's Makefile cannot handle spaces in paths"
}

select_targets() {
  cxx=${CXX:-g++-16}
  flags=(-std=c++26 -freflection -Wall -Wextra -Werror)
  targets=()
  bindings=false

  local format
  for format in "${formats[@]}"; do
    case $format in
      pdf | html | latex) targets+=("$paper.$format") ;;
      bindings) bindings=true ;;
      *)
        fail_build "unknown format: $format" \
          "(expected pdf, html, latex or bindings)"
        ;;
    esac
  done
}

prepare_output() {
  log=$root/build/log.txt
  tap=$out/tap_protocol.txt
  toolchain=$root/build/toolchain.txt
  { mkdir -p "$root/build" "$out" && : >"$log" && : >"$tap" &&
    : >"$out/.nojekyll"; } 2>/dev/null ||
    fail_build "cannot write $log or $out"
  exec 3>&1 4>&2 >"$tap" 2>>"$log"
  trap report_outcome EXIT
}

print_summary() {
  find "$out" -type f -printf '%s\n' | LC_ALL=C awk -v start="$started" \
    -v end="$(date +%s.%N)" '
      { bytes += $1 }
      END {
        printf "%.2f seconds. %d files. %.2f KiB.\n",
          end - start, NR, bytes / 1024
      }'
}

report_failure() {
  grep -E '^(Bail out!|not ok )' "$tap" || true
  echo "Failed, see ${tap#"$root"/}"
}

report_outcome() {
  if [[ $? -eq 0 ]]; then
    print_summary >&3
  else
    report_failure >&4
  fi
}

stale_output() {
  local output=$1 input
  shift
  [[ -e $output ]] || return 0
  for input in "$@"; do
    [[ -e $input && ! $input -nt $output ]] || return 0
  done
  return 1
}

quote_lines() {
  awk -v prefix="$1" -v root="$root/" '{
    while ((at = index($0, root)) > 0)
      $0 = substr($0, 1, at - 1) substr($0, at + length(root))
    print prefix $0
  }'
}

quiet_step() {
  local label=$1 start
  shift
  start=$(wc -l <"$log")
  echo "\$ $*" >>"$log"
  "$@" >>"$log" 2>&1 && return 0
  open_stream
  tail -n "+$((start + 1))" "$log" | quote_lines "# "
  echo "Bail out! $label failed, see build/log.txt"
  exit 1
}

fetch_framework() {
  [[ ! -f $root/wg21/flat.mk ]] || return 0
  quiet_step "submodule install" git -C "$root" submodule update --init
}

framework_make() {
  local label=$1 directory=$2 wg21=$root/wg21 file reuse=()
  shift 2
  for file in "$wg21"/deps/pandoc/* "$wg21"/deps/python \
    "$wg21"/data/refs.py "$wg21"/data/srefs.py; do
    [[ ! -e $file ]] || reuse+=(-o "$file")
  done

  quiet_step "$label" make --no-print-directory -C "$directory" \
    -f "$wg21/flat.mk" "${reuse[@]}" \
    "DEFAULTS=$root/config/pandoc_defaults.yaml" "$@"
}

paper_metadata() {
  sed -n '2,/^---$/{/^title:/d;/^---$/d;p}' "$proposal"
}

front_matter() {
  echo "---"
  echo "title: '${1//\'/\'\'}'"
  paper_metadata
  echo "---"
}

build_paper() {
  [[ ${#targets[@]} -gt 0 ]] || return 0

  framework_make "paper build" "$paper_dir" "OUTDIR=$out" "${targets[@]}"
}

build_index() {
  [[ " ${targets[*]} " == *" $paper.html "* ]] || return 0

  local page=$root/build/index.md
  if stale_output "$page" "$root/README.md" "$proposal" \
    "$root/config/pandoc_style.css" "$script"; then
    {
      front_matter "$(sed -n '1s/^# //p' "$root/README.md")"
      sed -e 1d -e 's/^## /# /' "$root/README.md"
    } >"$page"
  fi
  framework_make "index build" "$root/build" "OUTDIR=$out" index.html
}

record_site() {
  local user=whatchamacallem
  site=https://$user.github.io/$(basename "$root")
  site_record=$root/build/site.txt
  [[ -f $site_record && $(<"$site_record") == "$site" ]] ||
    echo "$site" >"$site_record"
}

record_toolchain() {
  local current
  current=$("$cxx" --version; rustc --version; echo "$out")
  [[ ! $script -nt $toolchain &&
    $(<"$toolchain") == "$current" ]] || echo "$current" >"$toolchain"
}

copy_sources() {
  quiet_step "source copy" cp -u "$root"/interop.* "$root"/test.* "$out"
}

copy_license() {
  quiet_step "license copy" cp -u "$root/LICENSE.md" "$out"
}

compile_generator() {
  stale_output "$root/build/interop" "$root/interop.cpp" "$root/interop.hpp" \
    "$toolchain" || return 0
  quiet_step "generator build" "$cxx" "${flags[@]}" "$root/interop.cpp" \
    -o "$root/build/interop"
}

run_generator() {
  local output
  for output in "$out"/interop_generated.{rs,py,js,cpp,h}; do
    stale_output "$output" "$root/build/interop" || continue
    quiet_step "generator run" env -C "$out" "$root/build/interop"
    break
  done
  quiet_step "binding log" tail -n +1 \
    "$out"/interop_generated.{rs,py,js,cpp,h}
}

compile_shim() {
  stale_output "$root/build/libinterop.so" "$out/interop_generated.cpp" \
    "$out/interop.hpp" "$toolchain" || return 0
  quiet_step "shim build" "$cxx" "${flags[@]}" -shared -fPIC \
    "$out/interop_generated.cpp" -o "$root/build/libinterop.so"
}

install_koffi() {
  local package=koffi@3.3.4 stamp=$root/build/node_modules/.koffi-pin
  [[ ! -f $stamp || $(<"$stamp") != "$package" ]] || return 0
  rm -f "$stamp"
  quiet_step "koffi install" npm install --prefix "$root/build" \
    --prefer-offline --no-audit --no-fund --no-update-notifier "$package"
  echo "$package" >"$stamp"
}

build_bindings() {
  $bindings || return 0

  record_toolchain
  copy_sources
  compile_generator
  run_generator
  compile_shim
  install_koffi
}

compile_rust() {
  stale_output "$root/build/test_rs" "$root/test.rs" \
    "$out/interop_generated.rs" "$toolchain" || return 0
  RUSTC_VERSION=$(rustc --version) INTEROP_RS=$out/interop_generated.rs rustc \
    -Dwarnings "$root/test.rs" -L "$root/build" -l interop \
    -o "$root/build/test_rs"
}

test_rust() {
  compile_rust && "$root/build/test_rs"
}

compile_cpp() {
  stale_output "$root/build/test_cpp" "$root/test.cpp" \
    "$root/interop.hpp" "$out/interop_generated.h" "$toolchain" || return 0
  "$cxx" "${flags[@]}" -I "$out" "$root/test.cpp" -L "$root/build" \
    -l interop -o "$root/build/test_cpp"
}

test_cpp() {
  compile_cpp && "$root/build/test_cpp"
}

run_subtest() {
  local number=$1 name=$2 output=$root/build/stdout.txt
  local errors=$root/build/stderr.txt status=ok
  shift 2
  echo "# Subtest: $name"
  { "$@"; } >"$output" 2>"$errors" || status="not ok"
  quote_lines "    " <"$output"
  quote_lines "    # " <"$errors"
  [[ $status == ok ]] || grep -q '^1\.\.' "$output" ||
    echo "    1..$(grep -cE '^(not )?ok ' "$output")"
  echo "$status $number - $name"
  [[ $status == ok ]] || failed=1
}

run_tests() {
  failed=0
  open_stream
  if ! $bindings; then
    echo "1..0 # SKIP bindings not requested"
    return 0
  fi

  export LD_LIBRARY_PATH=$root/build
  echo "1..4"
  run_subtest 1 Rust test_rust
  run_subtest 2 Python env "PYTHONPATH=$out" \
    "PYTHONPYCACHEPREFIX=$root/build/__pycache__" python3 "$root/test.py"
  run_subtest 3 ECMAScript \
    env "NODE_PATH=$out:$root/build/node_modules" node "$root/test.js"
  run_subtest 4 C++ test_cpp
}

page_language() {
  case $1 in
    *.rs) echo rust ;;
    *.py) echo python ;;
    *.js) echo javascript ;;
    *.txt) echo tap ;;
    *) echo cpp ;;
  esac
}

write_page() {
  local name=$1 page=$root/build/$1.md view=$root/config/source_view.md text
  stale_output "$page" "$out/$name" "$proposal" "$view" \
    "$root"/config/pandoc_{tap.xml,style.css} "$site_record" "$script" ||
    return 0
  text=$(<"$view")
  text=${text//'$file_name'/"$name"}
  text=${text//'$site_url'/"$site"}
  text=${text//'$code_language'/"$(page_language "$name")"}
  text=${text//'$paper_metadata'/"$(paper_metadata)"}
  printf '%s\n' "${text//'$source_code'/"$(<"$out/$name")"}" >"$page"
}

build_pages() {
  local name pages=()
  for name in interop.hpp interop.cpp interop_generated.{cpp,h,rs,py,js} \
    tap_protocol.txt test.{rs,py,js,cpp}; do
    [[ -e $out/$name ]] || continue
    write_page "$name"
    pages+=("$name.html")
  done
  framework_make "page build" "$root/build" "OUTDIR=$out" "${pages[@]}"
}

main() {
  start_clock
  locate_root
  parse_arguments "$@"
  locate_proposal
  select_targets
  prepare_output
  fetch_framework
  record_site
  copy_license
  build_paper
  build_index
  build_bindings
  run_tests
  build_pages
  exit "$failed"
}

main "$@"
