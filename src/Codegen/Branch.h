#pragma once

#include <stddef.h>
#include <stdint.h>

namespace Branch {
constexpr ptrdiff_t Range = ptrdiff_t{1} << 25;

inline bool InRange(const uint32_t* from, const uint32_t* to) {
  ptrdiff_t offset = to - from;
  return offset >= -Range && offset < Range;
}

inline uint32_t Encode(const uint32_t* from, const uint32_t* to) {
  return 0x14000000 | ((to - from) & 0x03FFFFFF);
}

}  // namespace Branch
