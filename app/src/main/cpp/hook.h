//
// Created by aaaaaa on 2025/5/6.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void hook_thread_create(void);
void unhook_thread_create(void);

#ifdef __cplusplus
}
#endif