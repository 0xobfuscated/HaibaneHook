#pragma once

#include <stddef.h>

namespace CodePatch {
bool Write(void* address, const void* data, size_t size);

}  // namespace CodePatch
