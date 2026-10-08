load("@aspect_rules_lint//format:defs.bzl", "format_multirun")

#
# This BUILD file is derived from the aspect_rules_lint example:
# https://github.com/aspect-build/rules_lint/blob/main/example/tools/format/BUILD.bazel
#

# see an example of a format_multirun for all supported languages here:
# https://github.com/aspect-build/rules_lint/blob/main/example/tools/format/BUILD.bazel#L66
format_multirun(
    name = "format",
    c = "@llvm_toolchain_llvm//:bin/clang-format",
    cc = "@llvm_toolchain_llvm//:bin/clang-format",
    visibility = ["//visibility:public"],
)
