#pragma once

#include <stddef.h>
#include <stdint.h>

namespace Relocator {
uint32_t* Relocate(const uint32_t* code, size_t count);

}  // namespace Relocator
