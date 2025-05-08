# ThreadHook

### 简介

该项目旨在介绍如何使用字节开源的bhook框架，对线程创建进行native hook。

对于Native Hook技术，有GOT/PLT Hook、Trap Hook 以及 Inline Hook，本文介绍的是GOT/PLT Hook（Hook外部函数）。

##### 配置项目

在app层的build.gradle中，集成bhook。

```groovy
android {
  	...
      buildFeatures {
          viewBinding true
          prefab true
      }
      packagingOptions {
          pickFirst '**/libbytehook.so'
      }
}
dependencies {
  ...
  implementation libs.bytedance.bytehook
}
```

##### 编写Java代码

在ThreadHook类中声明本地方法

```kotlin
object ThreadHook {
    init {
        ByteHook.init() //初始化bhook框架
        System.loadLibrary("hacker") //加载自己写的hook库
    }

    fun init() {

    }
		//本地方法，开启hook
    external fun hook()
		//本地方法，关闭hook
    external fun unHook()
}    
```

##### 编写C/C++文件

在cpp目录下（若没有，则在main目录下自行新建，与java目录同级）创建C/C++文件，实现hook逻辑。

新建hook.h文件，声明两个方法，***hook_thread_create***和***unhook_thread_create***。

```c++
//hook.h
  
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void hook_thread_create(void);
void unhook_thread_create(void);

#ifdef __cplusplus
}
#endif
```

新建hook.cpp文件，实现上面声明的两个方法。

```c++
//hook.cpp

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
  	//hook app native层创建线程的方法pthread_create为我们自己写的pthread_create_hook
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
```

新建hacker.cpp文件，实现在ThreadHook类中定义的native方法。

```c++
//hacker.cpp

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
```

##### 配置CMakeLists.txt

```cmake
# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html.
# For more examples on how to use CMake, see https://github.com/android/ndk-samples.

# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.22.1)
project("hacker")
add_library(hacker SHARED hacker.cpp hook.cpp)
find_package(bytehook REQUIRED CONFIG)
target_link_libraries(hacker bytehook::bytehook log)
```

##### 配置CMakeLists文件及ndk

在app层的build.gradle文件中进行配置。

```groovy
android {
   defaultConfig {
       ...
        ndk {
           abiFilters 'armeabi-v7a', 'arm64-v8a'
        }
    }
    ...
    externalNativeBuild {
        cmake {
            path file('src/main/cpp/CMakeLists.txt')
            version '3.22.1'
        }
    }
}
```

##### 初始化Hook能力

```kotlin
//HookApplication

class HookApplication: Application() {
    override fun onCreate() {
        super.onCreate()
        ThreadHook.init()
    }
}
```

以上我们便完成了对native层创建线程的hook。我们调用```ThreadHook.hook()```即可开启对线程创建的hook，调用```ThreadHook.unhook()```即可关闭hook，恢复系统默认调用。

##### 开启hook

开启hook后，所有线程创建的都会走hook逻辑，表现为打印如下日志：
<img src="img/demo.png" align="left" />










#### 参考文档

[bhook快速集成](https://github.com/bytedance/bhook/blob/main/doc/quickstart.zh-CN.md)

#### Demo代码

https://github.com/midiao/ThreadHook

