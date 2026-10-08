load("@buildifier_prebuilt//:rules.bzl", "buildifier")

package(default_visibility = ["//visibility:public"])

# --- BEGIN user-managed ---
# Repo-specific Bazel/Gazelle customizations — preserved across re-bootstrap.
# Add gazelle:exclude directives and tweak the buildifier excludes here.
#
# NOTE: the buildifier_prebuilt macro joins exclude_patterns with `-o` and
# appends ONE trailing `-prune`; find binds that prune to only the last
# `-path`, so keep this to a single pattern if you need pruning to take effect.
_BUILDIFIER_EXCLUDES = ["./.git/*"]
# --- END user-managed ---

# Buildifier lints and autoformats bazel (Starlark) files.
#
#   bazel run //:buildifier.fix
#   bazel run //:buildifier.check
#
# Windows works from buildifier_prebuilt 8.5.1.4 on, which fixed the runner's
# argument escaping (keith/buildifier-prebuilt#168).

buildifier(
    name = "buildifier.check",
    exclude_patterns = _BUILDIFIER_EXCLUDES,
    lint_mode = "warn",
    mode = "diff",
)

buildifier(
    name = "buildifier.fix",
    exclude_patterns = _BUILDIFIER_EXCLUDES,
    lint_mode = "fix",
    mode = "fix",
)

# rules_lint integration - export linter configurations
alias(
    name = "format",
    actual = "//tools/format",
)

exports_files(
    [".clang-tidy"],
    visibility = ["//visibility:public"],
)
