#include "Interceptor/Interceptor.h"

#include "Codegen/Assembler.h"
#include "Codegen/Branch.h"

namespace Interceptor {

std::vector<Hook>& Hooks() {
  static std::vector<Hook> hooks;
  return hooks;
}

size_t Find(const void* address) {
  std::vector<Hook>& hooks = Hooks();
  for (size_t i = 0; i < hooks.size(); i++) {
    if (hooks[i].entry == address) {
      return i;
    }
  }

  return hooks.size();
}

bool IsHooked(const void* address) {
  return Find(address) != Hooks().size();
}

uint32_t* BranchDestination(uint32_t* from, uint32_t* to) {
  if (Branch::InRange(from, to)) {
    return to;
  }

  Assembler assembler;
  assembler.Jump(to);
  return assembler.Commit(from);
}

}  // namespace Interceptor
