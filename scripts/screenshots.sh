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
  shots=$root/screenshots
}

clear_shots() {
  mkdir -p "$shots"
  find "$shots" -mindepth 1 -delete
}

build_site() {
  "$root/scripts/build_proposal.sh" >/dev/null
}

capture_pages() {
  python3 - "$shots" "$site"/*.html <<'EOF'
import base64
import json
import pathlib
import subprocess
import sys
import tempfile

shots = pathlib.Path(sys.argv[1])
log = tempfile.TemporaryFile()
chrome = subprocess.Popen(
    ["sh", "-c", 'exec "$0" "$@" 3<&0 4>&1 </dev/null >/dev/null',
     "chrome-headless-shell", "--remote-debugging-pipe",
     "--window-size=1280,720", "--blink-settings=preferredColorScheme=1"],
    stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=log)
sent = 0
pending = b""


def give_up(problem):
    log.seek(0)
    sys.stderr.write(log.read().decode(errors="replace"))
    sys.exit(f"screenshots: {problem}")


def next_message():
    global pending
    while b"\0" not in pending:
        chunk = chrome.stdout.read1(1 << 20)
        if not chunk:
            give_up("chrome exited")
        pending += chunk
    message, pending = pending.split(b"\0", 1)
    return json.loads(message)


def send_command(method, session=None, **params):
    global sent
    sent += 1
    command = {"id": sent, "method": method, "params": params}
    if session:
        command["sessionId"] = session
    chrome.stdin.write(json.dumps(command).encode() + b"\0")
    chrome.stdin.flush()
    return sent


def await_reply(number, event=None):
    reply = None
    while reply is None or event:
        message = next_message()
        if message.get("id") == number:
            reply = message
        elif message.get("method") == event:
            event = None
    if "error" in reply:
        give_up(reply["error"]["message"])
    return reply["result"]


target = await_reply(send_command("Target.createTarget", url="about:blank"))
session = await_reply(send_command(
    "Target.attachToTarget", targetId=target["targetId"], flatten=True))
session = session["sessionId"]
await_reply(send_command("Page.enable", session))
for page in map(pathlib.Path, sys.argv[2:]):
    loaded = await_reply(
        send_command("Page.navigate", session, url=page.as_uri()),
        "Page.loadEventFired")
    if "errorText" in loaded:
        give_up(f"{page.name}: {loaded['errorText']}")
    image = await_reply(send_command("Page.captureScreenshot", session))
    (shots / f"{page.name}.png").write_bytes(base64.b64decode(image["data"]))
send_command("Browser.close")
chrome.wait()
EOF
}

render_pdfs() {
  local pdf
  for pdf in "$site"/*.pdf; do
    pdftoppm -png -f 1 -l 1 -singlefile -scale-to-x -1 -scale-to-y 720 \
      "$pdf" "$shots/$(basename "$pdf")"
  done
}

print_summary() {
  find "$shots" -type f -printf '%s\n' | LC_ALL=C awk -v start="$started" \
    -v end="$(date +%s.%N)" '
      { bytes += $1 }
      END {
        printf "%.2f seconds. %d files. %.2f KiB.\n",
          end - start, NR, bytes / 1024
      }'
}

main() {
  start_clock
  locate_root
  clear_shots
  build_site
  capture_pages
  render_pdfs
  print_summary
}

main
