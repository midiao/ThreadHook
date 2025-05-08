package com.codeedge.threadhook

import com.bytedance.android.bytehook.ByteHook

object ThreadHook {
    init {
        ByteHook.init()
        System.loadLibrary("hacker")
    }

    fun init() {

    }


    external fun hook()

    external fun unHook()
}