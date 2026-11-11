#!/usr/bin/env bash
# SPDX-FileCopyrightText: © 2026 Adrian Johnston.
# SPDX-License-Identifier: MIT
# This file is licensed under the terms of the LICENSE.md file.

set -euo pipefail

start_clock() {
  started=$(date +%s.%N)
}

locate_root() {
  root=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
  site=$root/staging
}

build_files() {
  "$root/scripts/build.sh" >/dev/null
}

push_site() {
  local revision tree commit
  revision=$(git -C "$root" describe --always --dirty)
  index=$(mktemp -u)
  trap 'rm -f -- "$index"' EXIT
  export GIT_INDEX_FILE=$index
  GIT_WORK_TREE=$site git -C "$root" add -A
  tree=$(git -C "$root" write-tree)
  commit=$(git -C "$root" commit-tree "$tree" -m "page $revision")
  git -C "$root" push -q --force origin "$commit:refs/heads/gh-pages"
  unset GIT_INDEX_FILE
  git -C "$root" update-ref -d refs/remotes/origin/gh-pages 2>/dev/null || true
}

skip_fetch() {
  git -C "$root" config --replace-all remote.origin.fetch \
    '^refs/heads/gh-pages' '^\^refs/heads/gh-pages$'
}

print_summary() {
  find "$site" -type f -printf '%s\n' | LC_ALL=C awk -v start="$started" \
    -v end="$(date +%s.%N)" '
      { bytes += $1 }
      END {
        printf "%.2f seconds. %d files. %.2f KiB.\n",
          end - start, NR, bytes / 1024
      }'
}

print_address() {
  cat "$root/build/site.txt"
}

main() {
  start_clock
  locate_root
  build_files
  push_site
  skip_fetch
  print_summary
  print_address
}

main
