HaibaneHookPath := $(abspath $(call my-dir))

include $(CLEAR_VARS)

LOCAL_MODULE := haibanehook
LOCAL_SRC_FILES := \
    $(HaibaneHookPath)/src/HaibaneHook.cpp \
    $(HaibaneHookPath)/src/Codegen/Assembler.cpp \
    $(HaibaneHookPath)/src/Codegen/Relocator.cpp \
    $(HaibaneHookPath)/src/Interceptor/Interceptor.cpp \
    $(HaibaneHookPath)/src/Memory/CodeAllocator.cpp \
    $(HaibaneHookPath)/src/Memory/CodePatch.cpp
LOCAL_C_INCLUDES := $(HaibaneHookPath)/include $(HaibaneHookPath)/src
LOCAL_EXPORT_C_INCLUDES := $(HaibaneHookPath)/include
LOCAL_CFLAGS := -g0

include $(BUILD_STATIC_LIBRARY)
