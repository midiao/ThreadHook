#include <android/api-level.h>
#include <android/log.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <cinttypes>
#include <jni.h>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <sys/system_properties.h>
#include <unistd.h>

#include "bytehook.h"
#include "hook.h"

extern "C" JNIEXPORT void JNICALL
Java_com_codeedge_threadhook_ThreadHook_hook(
        JNIEnv *env,
        jobject thiz) {
    hook_thread_create();
}

extern "C" JNIEXPORT void JNICALL
Java_com_codeedge_threadhook_ThreadHook_unHook(
        JNIEnv *env,
        jobject thiz) {
    unhook_thread_create();
}