# gcc-toolchain-issue215-repro

<!-- --- BEGIN user-managed --- -->
A repro for [f0rmiga/gcc-toolchain#215](https://github.com/f0rmiga/gcc-toolchain/issues/215):
what `system_include_paths` and `external_include_paths` do with the hermetic GCC
toolchain on Bazel 9.

Versions: Bazel 9.3.0, rules_cc 0.2.26, gcc_toolchain 0.13.0 (GCC 15.2.0), abseil-cpp
20260817.0. `--config=gcc_hermetic` selects the toolchain and adds `-Wall`; `.bazelrc`
adds `-Werror`. The patch
`tools/cpp/patches/gcc_toolchain_0.13.0_external_include_paths.patch` adds the
`external_include_paths` feature of
[f0rmiga/gcc-toolchain#242](https://github.com/f0rmiga/gcc-toolchain/pull/242) to the
toolchain.

The targets in `modules/app`:

- `own_header` includes the header of `//modules/mylib`, which uses `includes`. The
  header has a `-Wsign-compare` warning.
- `absl_angle` has `#include <absl/strings/ascii.h>`, as in the report. It compiles with
  `-Werror=useless-cast`, which abseil's headers trigger.
- `absl_quote` is the same with `#include "absl/strings/ascii.h"`.

```shell
bazel build --config=gcc_hermetic //modules/app:<target>
bazel build --config=gcc_hermetic --config=system_include_paths //modules/app:<target>
bazel build --config=gcc_hermetic --config=external_include_paths //modules/app:<target>
```

| Run | `own_header` | `absl_angle` | `absl_quote` |
| --- | --- | --- | --- |
| No feature | fails: warning in our header | fails: `No such file or directory` | fails: warning in abseil |
| `system_include_paths` | **builds**: the warning in our header is hidden | fails: `No such file or directory` | fails: warning in abseil |
| `external_include_paths` | fails: warning in our header | builds | builds |
| `external_include_paths`, gcc_toolchain 0.13.0 without the patch | fails: warning in our header | fails: `No such file or directory` | fails: warning in abseil |

- On Bazel 9 the dirs of the `includes` attribute come in through `-I`
  ([bazelbuild/bazel@2dde0d5](https://github.com/bazelbuild/bazel/commit/2dde0d53a3c867b3b862c951092b25bae623d409)).
  `system_include_paths` brings back `-isystem` for them. Set with `--features`, it does
  so for every target, so it also hides the warning in our own header.
- abseil has no `includes` attribute. Its headers are found relative to the root of its
  repository, which is an `-iquote` dir. `system_include_paths` does not change that dir:
  the angle include is not found, and the quote include still reports the warning.
- `external_include_paths` makes the root of each external repository an `-isystem` dir,
  and does not touch first-party targets. gcc_toolchain 0.13.0 does not define the
  feature, so without the patch the flag does nothing.
<!-- --- END user-managed --- -->

## Project Layout

- `modules/` — your code: modules, services, and apps
- `MODULE.bazel` — module definition, segmented by language for easy pruning
- `tools/` — toolchains, formatters, and per-language dependency configuration

Add a module under `modules/`, then `bazel build //...` and
`bazel test //...` to confirm it wires up. The toolchains are already
configured — no per-language setup required.

## Install Bazelisk

Toolchains are hermetic, so Bazelisk is the only thing you need installed — it
reads `.bazelversion` and fetches the matching Bazel release on demand.

```bash
# Auto: system install (apt/.deb) when root or passwordless sudo is available,
# otherwise a no-sudo install into ~/.local/bin (PATH wired into your shell rc)
tools/setup/install_bazelisk.sh

# Force one mode or the other
tools/setup/install_bazelisk.sh --user      # no sudo, ~/.local/bin
tools/setup/install_bazelisk.sh --system    # apt/.deb, prompts for sudo
```

Linux only. Run `bazel version` to verify (restart your shell first if the
installer added `~/.local/bin` to your PATH).

Downloads go through `tools/setup/download_lib.sh`, which resumes from the bytes
already on disk instead of restarting when a transfer stops making progress.
Behind a slow or stalling proxy, raise `DOWNLOAD_STALL_SECONDS` (default 30, the
seconds under `DOWNLOAD_MIN_SPEED` before an attempt is abandoned) or
`DOWNLOAD_MAX_STALLED_RETRIES` (default 5).

## Common Commands

```bash
# List runnable targets — apps, buildifier, venv (plain `//...` lists everything)
bazel query 'kind("(py|cc|go|java|rust)_binary|buildifier|_venv", //...)'

# Build / test everything
bazel build //...
bazel test //...                     # excludes lint tests — run lint separately

# Format source (all languages), then Bazel/Starlark files
bazel run //:buildifier.fix
bazel run //:format
```

### Sync dependencies

After adding or removing a module extension repo in a `*.MODULE.bazel` segment, sync
the `use_repo()` calls.

```bash
bazel mod tidy                                                        # Bazel  — use_repo() calls in the MODULE segments
```

## Local Disk Cache

Faster local builds, shared across every Bazel project you build. Add to your
**user-global `~/.bazelrc`** (not this repo — it applies to all your workspaces):

```bash
build --disk_cache=~/.cache/bazel-disk
build --experimental_disk_cache_gc_max_size=15G   # bounded; auto-GC'd when idle (Bazel 7.4+)
```

The same action-cache (AC/CAS) mechanism as the remote cache, on local disk:
identical actions (e.g. a shared protobuf compile) run once and are reused
everywhere, and `bazel clean` becomes cheap to recover from. Trades disk for
speed — tune the size to taste.
