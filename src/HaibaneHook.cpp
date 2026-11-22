#include "haibanehook.hpp"

#include <vector>

#include "Codegen/Branch.h"
#include "Codegen/Relocator.h"
#include "Interceptor/Interceptor.h"
#include "Memory/CodePatch.h"

namespace HaibaneHook {

int Hook(void* target, void* replacement, void** original) {
  uint32_t* entry = static_cast<uint32_t*>(target);
  if (!entry || !replacement || Interceptor::IsHooked(entry)) {
    return -1;
  }

  uint32_t* destination = Interceptor::BranchDestination(entry, static_cast<uint32_t*>(replacement));
  uint32_t* relocated = Relocator::Relocate(entry, 1);
  if (!destination || !relocated) {
    return -1;
  }

  if (original) {
    *original = relocated;
  }

  Interceptor::Hook hook = {entry, *entry};
  uint32_t branch = Branch::Encode(entry, destination);
  if (!CodePatch::Write(entry, &branch, sizeof(branch))) {
    return -1;
  }

  Interceptor::Hooks().push_back(hook);
  return 0;
}

int Unhook(void* target) {
  std::vector<Interceptor::Hook>& hooks = Interceptor::Hooks();
  size_t index = Interceptor::Find(target);
  if (index == hooks.size()) {
    return -1;
  }

  Interceptor::Hook& hook = hooks[index];
  if (!CodePatch::Write(hook.entry, &hook.original, sizeof(hook.original))) {
    return -1;
  }

  hooks.erase(hooks.begin() + index);
  return 0;
}

}  // namespace HaibaneHook
