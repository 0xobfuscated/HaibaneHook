# HaibaneHook

A (very) minimal inline hooking library for arm64 Android. It has <400 lines of code and the compiled libhaibanehook.a is only 38 KiB big.

## Usage

`haibanehook.hpp` declares two functions:

```cpp
int Hook(void* target, void* replacement, void** original);
int Unhook(void* target);
```

`Hook` redirects `target` to `replacement` and stores a copy of the unhooked function in `original`, just like other frameworks like Dobby. `Unhook` restores the code that `Hook` overwrote.

```cpp
#include <stdint.h>
#include <haibanehook.hpp>

void (*orig_GameMain_update)(void*, float) = nullptr;

void GameMain_update(void* self, float delta) {
  orig_GameMain_update(self, delta);
}

void Install() {
  void* target = reinterpret_cast<void*>(base + 0x123456);
  HaibaneHook::Hook(target, (void*)GameMain_update, (void**)&orig_GameMain_update);
}

void Uninstall() {
  HaibaneHook::Unhook(reinterpret_cast<void*>(base + 0x123456));
}
```
Both functions return 0 on success and -1 on failure.

## Building

The library needs C++17 and builds for `arm64-v8a` only.

Run `ndk-build` to build `obj/local/arm64-v8a/libhaibanehook.a`. The settings are in
`jni/Application.mk`.

To build it into another project, include `HaibaneHook.mk` from that project's `Android.mk` and link the library:

```make
include $(LOCAL_PATH)/../deps/HaibaneHook/HaibaneHook.mk
# ...
LOCAL_STATIC_LIBRARIES := haibanehook
```

## License
Apache License 2.0
