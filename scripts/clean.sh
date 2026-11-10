#!/bin/sh
# SPDX-FileCopyrightText: © 2026 Adrian Johnston.
# SPDX-License-Identifier: MIT
# This file is licensed under the terms of the LICENSE.md file.

set -eu

cd "$(dirname "$(readlink -f "$0")")/.."

# Nukes the files in .gitignore except tmp/
[ ! -d .git ] || git clean -Xdf -e '!tmp/' -e '!tmp/**'
