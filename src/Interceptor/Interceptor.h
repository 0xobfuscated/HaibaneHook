#pragma once

#include <stddef.h>
#include <stdint.h>

#include <vector>

namespace Interceptor {

struct Hook {
  uint32_t* entry;
  uint32_t original;
};

std::vector<Hook>& Hooks();
size_t Find(const void* address);
bool IsHooked(const void* address);
uint32_t* BranchDestination(uint32_t* from, uint32_t* to);

}  // namespace Interceptor
