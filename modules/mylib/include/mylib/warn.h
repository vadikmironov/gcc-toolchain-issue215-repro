#pragma once

// -Wall (-Wsign-compare): comparison of integers of different signedness.
inline bool mylib_less(int a, unsigned b) { return a < b; }
