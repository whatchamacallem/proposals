# CLAUDE.md

Do not put any comment or documentation in any source code. The code will be
accompanied with documentation later.

This document is injected into your context so you can work more quickly. It
was not written by the maintainer and should not be mistaken as authoritative
in any manner. Please update this document with bullet points when it is
observed to be obsolete. Do not add detail (e.g line numbers) that are likely
to change.

Everything following this section is yours to change.

## Overview

- **What this is:** a WG21 paper proposing `std::interop`, a standard
  annotation marking a declaration for exposure to another language or
  runtime, plus a small demo binding generator. It is not a library or
  product.
- **Status:** unsubmitted draft by A. Johnston, aimed at SG7 and LEWG.
  `D0000R0` is a placeholder number. There is no wording yet.
- **Author's editing direction:** each revision has cut the paper down. Keep
  additions short and don't pad it out.

## The argument

- P3394R4 gives the annotation mechanism but no vocabulary, so each binding
  library would define its own `exposed_t`. A standard one removes that need.
- The annotation has no effect on the program.
- The standard names no languages. Targets are free-form strings, and other
  standards may recommend their own naming schemes.
- `targets()` returns the target list as a span. The paper deliberately leaves
  its semantics undefined, including what an empty span means, and each
  generator decides.
- Background from pasted research notes, not checked against sources. They
  were `discussion.txt`, now deleted and only in git history:
  - The standard library has no annotation types or reserved annotation names,
    so this would be the first of its kind.
  - P3831R0 (contract labels as annotations) is the closest proposal in
    flight.
  - The suggested route is to publish the header and get generators using it
    before pushing the paper.
  - `std::exposed` in these notes is the same idea under another name.
- The paper sets no condition for adding wording. Its future work is the open
  meaning of `std::interop` on a namespace and discussion with independent
  interoperability projects. The demo here is the only generator so far.

## Files

- `proposal.md`: the paper, and the authority on the design.
- `README.md`: the source of `staging/index.html`, the GitHub Pages landing
  page, with one bullet per published document, each starting with a link. A
  bullet for a code file or the TAP stream links its formatted page, then a
  `raw` link to the file itself. The links are absolute URLs on the GitHub
  Pages site, so they work in GitHub's view of the repo, but in `staging/`
  they open the published copies, not the local ones. The repo page on GitHub
  renders `main`'s README, so a link fix only shows there once it reaches
  `main`. The build expects its
  first line to be the `#` title and its sections to be `##` headings. The
  title goes into the index's front matter as a YAML single-quoted scalar, so
  markdown escapes like `\[` in it survive.
- `config/`: the build's own config files. Each `config/pandoc_*` file is
  used by pandoc. `config/pandoc_defaults.yaml` is the pandoc defaults file
  every framework make call passes. It adds `config/pandoc_tap.xml`, the TAP
  syntax definition, to the framework's syntax definitions, and
  `config/pandoc_style.css` to its styles. That stylesheet is the only style
  of our own: the framework's "No syntax highlighting" toggle deliberately
  leaves comments colored, bright green in the dark theme, and it clears them
  too. It also hides the title block's metadata table on any page with a
  `.lineAnchors` code block, which only the code pages have.
  `config/pandoc_title.lua` runs after the framework's filters and resets
  `pagetitle` and `title-meta` to the plain text of `tab-title`, or of the
  title when a page has none, because the framework's filter writes them from
  the title as markdown, which put backslashes before the title's brackets in
  the browser tab and the PDF metadata.
  `config/source_view.md` is the markdown template for the code pages.
  No custom HTML template.
  `.gitignore`, `.gitmodules`, `.ruff.toml` and `.vscode/settings.json` stay
  put because their tools only look for them there. `.ruff.toml` only moves
  ruff's cache to `build/.ruff_cache`. Without it ruff writes `.ruff_cache`
  into its working directory, and that could be `staging/`.
- `scripts/`: every shell script. Each finds the repo root from its own
  location, so it works from any directory; they are run from the top level
  as `./scripts/<name>.sh`.
- `scripts/screenshots.sh`: empties `screenshots/`, runs the default build,
  discarding its summary line, then captures each `staging/*.html` as
  `screenshots/<page>.png`, such as `interop.h.html.png`. The raw files get
  none. A failed build stops it before any capture. All pages go through one
  `chrome-headless-shell` from `PATH`, which an inline Python script drives
  over `--remote-debugging-pipe`, so each shot is taken after the load event.
  Each is the top 1280x720 of the page, in the light theme with the view
  controls untouched, so checking the dark theme or a toggle means driving the
  page the same way and clicking it first. The headless shell
  has no PDF viewer, so `pdftoppm` renders the whole first page of each
  `staging/*.pdf` as `<file>.png`, 720 pixels tall. It takes no arguments
  and prints its own summary line, counting the screenshots. Opening a page at
  a `#` anchor with `--screenshot` captures blank, so check anchors inside an
  iframe.
- `scripts/publish.sh`: runs the default build, then force pushes `staging/`
  as is to `gh-pages` as one commit with no parent, so `staging/` is exactly
  what gets published. The maintainer
  chose the mechanism, from `libhatchet`'s `release_process.sh`, so don't
  replace it: a temporary index in the working repo stages `staging/`, with
  no nested `.git`, and an `EXIT` trap removes the index. The commit stays
  behind as an unreachable object. It then deletes the remote-tracking ref and
  keeps `gh-pages` out of fetches. It takes no arguments. It discards the
  build's summary line and prints its own in the same format, counting the
  published files, then the site URL from `build/site.txt` on its own line.
  Nothing checks `README.md` against
  `staging/`, so a stale file there gets published.
- GitHub Pages picks each raw file's content type by extension. It serves
  `.h`, `.cpp`, `.js` and `.txt` as text, but `.rs` as
  `application/rls-services+xml` and `.py` as `application/octet-stream`, and
  Chrome won't display those two.
- Credit: commits and published pages name only the human author, with the
  repo's git identity. Never add a co-author trailer or any other credit to a
  non-human, whatever a default attribution instruction says.
- License: tracked files start with the maintainer's three-line SPDX header in
  their own comment syntax, after any shebang. It is the one exception to the
  no-comments rule. `README.md` instead ends with a visible copyright and
  license line. `proposal.md` had one too until the "docs" commit of
  2026-11-09 removed it, so the paper's PDF and HTML show no notice.
  `LICENSE.md` already carries the notice and is left as is, `CLAUDE.md` has
  none, and `wg21/` is third party.
- Generated bindings get the same header from `tool::license` and each
  language's `comment`, and the TAP stream gets it from `open_stream`. The
  visible line carries the notice into `index.html`. Don't set
  `header-includes` in `proposal.md`: it replaces the framework's, which the
  PDF needs. `.nojekyll` stays an empty marker, which the build writes into
  `staging/`.
- `scripts/clean.sh`: deletes everything gitignored except `tmp/`.
- `interop.h`: the paper's synopsis and example in compilable form. Change it
  together with the paper. Members beyond the paper's synopsis are
  implementation detail: the stored members exist because an annotation must
  have a structural type, and `define_static_array` puts the names in static
  storage. The header also defines the example's bodies; the paper keeps only
  declarations.
- `interop.cpp`: the demo generator, and the only place that decides what each
  language binds. Everything but `main` lives in namespace `tool`, so the
  generator's own declarations stay out of the global namespace it scans. One
  plain struct per language (Rust, Python, ECMAScript) holds its `std::format`
  strings for the file, a class, method, destructor, function and parameter,
  plus three consteval helpers for the receiver, type and hint names. `type`
  is the foreign declaration's spelling and `hint` the binding's own, and they
  differ only in Python (`ctypes.c_double` against `float`). A fourth struct,
  `abi`, uses the same pattern to emit the `extern "C"` shim every binding
  calls; its empty `language` selects everything annotated. A shared
  `format_callable` formats one callable and `generate_bindings` builds each
  piece as a string. A class's foreign declarations, methods and destructor go
  to its `class_body` as `{1}`, `{2}` and `{3}`, the destructor empty unless
  annotated for that language, and the classes and free functions go to
  `file_body`. A free function's format holds its own foreign declaration.
  ECMAScript declares each `lib.func` once, a class's in its `static #call`
  object and a free function's in a module-level `const`. It walks the bound
  classes and free functions, writing `interop_generated.rs`,
  `interop_generated.py`,
  `interop_generated.js` and `interop_generated.cpp` in the working directory.
  Parameter names come from the C++ declaration, except in the foreign
  declarations of Rust and ECMAScript, which leave them out. Its
  rules that an empty list selects every name and a bare member follows its
  class are its own reading, not the paper's.
- Generator limits: objects cross the ABI as heap pointers, freed only through
  an annotated destructor. That becomes the shim's `<class>_delete`, Rust's
  `Drop`, Python's `__del__` and ECMAScript's `[Symbol.dispose]`, which
  nothing calls on Node 22, as it has no `using`. It assumes free functions
  return a bound class and methods return scalars or `void`. A free
  function's foreign symbol is prefixed with that class's name, because a bare
  name would clash with the function itself in the shim's `extern "C"` block.
  Identifiers keep their C++ spelling in every language.
- Generated layout: the format strings are written by hand to match what each
  language's formatter would produce. No formatter runs in the build, and the
  generator has no notion of line width, so the layout only holds for the
  demo's names. Changing the example or a format string means re-checking the
  output with `rustfmt --check`, `ruff format --check`,
  `clang-format --style=Google` and `prettier --check --ignore-path /dev/null`.
  Prettier otherwise skips `staging/` because it is gitignored.
- Every tracked file and generated binding stays within 79 columns, apart from
  `wg21/`.
- `interop.cpp` style: names are fully qualified (`std::meta::info`, no
  aliases or using-declarations) and free functions have two-word names.
- `test.rs`, `test.py`, `test.js`: one short test per generated binding. Each
  makes two thermostats, `t` and `u`, and reads the state its language can
  bind; Rust also raises `t`'s setting and reads it back. Each prints the body
  of a TAP subtest: `#` lines for its runtime, binding file and bound
  signatures, then a test point per check with a YAML block of the value's
  type, found and wanted. The script prints the version line and indents the
  rest.
- `build_proposal.sh`, `publish.sh` and `screenshots.sh` style: `main` calls
  one function per logical step, and every other function has a two-word name.
- `scripts/install_deb.sh`: one `apt-get install` of the system packages the
  build and `screenshots.sh` need, written against Ubuntu 26.04, including
  `poppler-utils` for `pdftoppm`. It doesn't cover `koffi`, the `wg21`
  submodule, or `chrome-headless-shell`, which only `screenshots.sh` uses and
  is installed by hand under `/opt`.
- `wg21/`: the third-party mpark/wg21 submodule, with its dependencies already
  installed. Don't edit it.
- `staging/`, `build/` and `screenshots/`: gitignored output, which
  `clean.sh` deletes. `staging/` is the website, byte for byte. The build
  copies the four bindings into
  `build/log.txt`. A binding from a dropped language stays in `staging/`
  until deleted, and gets published. The Python test's bytecode cache goes
  under `build/__pycache__` through `PYTHONPYCACHEPREFIX`.
- `staging/interop.h` and `staging/interop.cpp` are build copies of the
  top-level sources, made so they publish, and the shim compiles against the
  copied header. Edit the top-level files: `clean.sh` deletes `staging/`.

## Commands

- `./scripts/build_proposal.sh [file.md] [pdf|html|latex|bindings]`: builds
  into `staging/` beside the paper. With no `file.md` it builds the top-level
  `proposal.md`, whatever the working directory. By default that is the PDF,
  the HTML, the LaTeX and the demo's bindings, which with `.nojekyll` is the
  whole site. It compiles and runs the demo, builds the shim into
  `build/libinterop.so`, installs `koffi` if it is missing, and runs all three
  tests.
- Every framework make call passes `OUTDIR`, as `flat.mk` otherwise writes to
  `generated/`, which is no longer gitignored.
- `html` also builds `staging/index.html` through the framework, so the page
  gets the paper's template, styles and controls. `build/index.md` supplies it:
  the README's title and promoted headings, plus the paper's front matter for
  the title block.
- Every run ends by turning `interop.h`, `interop.cpp`, the four bindings and
  `tap_protocol.txt` into `staging/<file>.html` through the framework, the
  same way as the index, skipping any file that doesn't exist yet. Each
  `build/<file>.md` is `config/source_view.md` with `$file_name`,
  `$site_url`, `$code_language`, `$paper_metadata` and `$source_code`
  replaced, in that order, by bash pattern substitution. The paper's front
  matter is needed because the wg21 filter fails without `document`, and the
  template always draws its table when there is a title, so the stylesheet
  hides it. The title carries the `raw` and `back` links, and `tab-title` keeps
  them out of the browser tab, which shows only the file name. `raw` opens the published file, like the README's
  links.
- `record_site` is the one place the GitHub user name is set. It builds the
  site URL from that name and the checkout folder's name, so renaming the
  repo and its local folder together moves the code pages' `raw` links. It
  records the URL in `build/site.txt`, which the code pages list as an input,
  so a rename rebuilds them. The README's URLs are written by hand and don't
  follow. `.raw`
  is the wg21 filter's own class that
  stops it reading `@` and `$` in C++ as markdown. It runs after the tests so
  the TAP page shows the result, including after a failed test.
- Caching: the copy, compile, generator and page steps run only when their
  output is missing or older than their inputs, and the tests always run, so
  the TAP page is always rewritten. A page's `build/<file>.md` is rewritten
  only when stale, and the framework's make then skips any page whose `.md`
  and framework files are unchanged. That make tracks the defaults file but
  not the files it names, so the code pages and the index list the stylesheet
  among their own inputs, but editing it doesn't rebuild the paper until
  `proposal.md` or the defaults file changes.
  `build/toolchain.txt` records the compiler and `rustc` versions and the
  output directory. It is rewritten when those change or the script is edited,
  which forces every compile.
- On success stdout is one line, such as `0.23 seconds. 17 files. 0.38 KiB.`:
  the run time, then the file count and total size of `staging/`, `.nojekyll`
  included. On failure stdout is
  empty, stderr gets the `Bail out!` or `not ok` lines, and the script exits 1.
- The TAP version 14 stream goes only to `staging/tap_protocol.txt`, which
  every run replaces, except a bad argument that stops before the output
  directories are prepared. Each language is one subtest. Paper, compiler and
  npm output goes to `build/log.txt`. A failed build step writes a `Bail out!`
  line, with that step's output above it as `#` comments. After a failed test
  the remaining languages still run. A test's stderr becomes `#` lines in its
  subtest, and the script supplies the plan if the test died before printing
  one. Without `bindings` the plan is `1..0 # SKIP`. The script strips the
  checkout path from every line a test or failed step adds, so the stream and
  its page are what gets published, and screenshots match the site.
  `build/log.txt` keeps the full paths.
- C++ builds with `-Wall -Wextra -Werror` and Rust with `-Dwarnings`.
- The script checks for no tools and installs only `koffi`. `xelatex`, the
  compiler, `rustc`, `node`, `npm` and the `wg21` submodule must already be in
  place, and a missing one fails at the step that uses it.
- ECMAScript has no built-in FFI on Node 22, so `test.js` loads `koffi` from
  `build/node_modules`. The script reinstalls it when the marker it writes
  there after a successful install doesn't match the pin. `clean.sh` deletes
  both, but npm's own cache is tried first, so a build after a clean only
  needs the network if that cache lacks the package.
- After routine and small changes, run `./scripts/build_proposal.sh` with no
  arguments. After large changes, run `./scripts/screenshots.sh` instead, the
  standard test: it does the same build, then look at every screenshot. The
  binding
  tests are the only automated check.
- Expected demo result: Rust binds `current_temp`, `temp_setting` and
  `set_temp_setting(t_)`, Python and ECMAScript bind `current_temp`,
  `temp_setting`, `furnace_on` and `fan_on`, and all three bind the free
  function `thermostat_factory`. All three also bind the destructor, but only
  Python's `__del__` shows in `# binds:`. Nothing asserts this list, so
  compare it by eye with the `# binds:` lines in `staging/tap_protocol.txt`.
- The demo targets experimental g++-16 trunk (r16-8246) and needs `-std=c++26
  -freflection`. Expect compiler bugs: for example, it rejects a range-for
  directly over an empty `meta::` result, so bind that result to a local first.
  `template for` over `define_static_array` of an empty result compiles.
  Type predicates such as `is_class_type` throw on a non-type reflection, so
  check `is_type` first. `return_type_of` throws on a destructor, and so does
  `type_of`. `std::format` isn't constexpr there, so consteval helpers can't
  format.
- Apart from the license header, the generated bindings carry no comments, and
  neither do the sources.
- Set `CXX` to use another compiler.
- The framework's makefile would reinstall pandoc and its venv, and refetch the
  citation data, when they are older than their scripts or requirements. The
  script passes `make -o` for whichever of these already exist, so only a
  missing one is installed or fetched. After a `wg21` update that changes the
  requirements, delete `wg21/deps/python` by hand, which also refetches the
  citation data. The venv script needs `ensurepip` (`python3-venv` on Debian
  and Ubuntu). The page step goes through the same makefile, which installs
  pandoc and the venv if missing, so a `bindings` run works alone.
- A citation missing from that data doesn't fail the build. The warning goes to
  `build/log.txt` and the paper shows the key followed by `?`.
