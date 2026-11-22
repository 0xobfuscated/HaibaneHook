#pragma once

namespace HaibaneHook {

int Hook(void* target, void* replacement, void** original);
int Unhook(void* target);

}  // namespace HaibaneHook
