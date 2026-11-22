#pragma once

#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

namespace Page {

inline size_t Size() {
  return sysconf(_SC_PAGESIZE);
}

inline uintptr_t Align(uintptr_t value, uintptr_t alignment) {
  return (value + alignment - 1) & ~(alignment - 1);
}

}  // namespace Page
