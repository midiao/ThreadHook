#include <bits/pthread_types.h>
#include <android/log.h>
#include <atomic>
#include "hook.h"
#include "bytehook.h"

#define  LOG_TAG    "HOOK"
#define  ALOG(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

static bytehook_stub_t stub = NULL;
std::atomic<bool> thread_hooked;

int pthread_create_hook(pthread_t *thread, const pthread_attr_t *attr,
                        void *(*start_routine)(void *), void *arg) {
    BYTEHOOK_STACK_SCOPE();
    __android_log_print(ANDROID_LOG_DEBUG, "HOOK", "hook thread create start");
    size_t result = BYTEHOOK_CALL_PREV(pthread_create_hook, thread, attr, *start_routine, arg);
    __android_log_print(ANDROID_LOG_DEBUG, "HOOK", "hook thread create end");
    return result;
}

void hook_thread_create() {
    if (thread_hooked) {
        ALOG("已开启hook，无需重复开启");
        return;
    }
    ALOG("开启hook");

    thread_hooked = true;
    stub = bytehook_hook_single(
            "libart.so",
            nullptr,
            "pthread_create",
            reinterpret_cast<void *>(pthread_create_hook),
            nullptr,
            nullptr);
}

void unhook_thread_create() {
    ALOG("关闭hook");
    bytehook_unhook(stub);
    stub = nullptr;
    thread_hooked = false;
}
