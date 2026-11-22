#pragma once

#include <stddef.h>
#include <stdint.h>

namespace CodeAllocator {
uint32_t* Allocate(size_t count, const uint32_t* near);

}  // namespace CodeAllocator
