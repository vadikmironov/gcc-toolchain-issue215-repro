// As in the report of gcc-toolchain#215.
#include <absl/strings/ascii.h>

int main() { return absl::ascii_isdigit('1') ? 0 : 1; }
