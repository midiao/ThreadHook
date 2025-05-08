package com.codeedge.threadhook

import android.app.Application

class HookApplication: Application() {
    override fun onCreate() {
        super.onCreate()
        ThreadHook.init()
    }
}