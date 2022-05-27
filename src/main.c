/*
 * @Author       : Zeepunt.H
 * @Date         : 2022-03-12 13:15:02
 * @LastEditTime : 2022-05-28 02:15:05
 * Gitee : https://gitee.com/zeepunt
 * Github: https://github.com/Recaa
 *  
 * Copyright (c) 2022 by Zeepunt.H, All Rights Reserved. 
 */

#include <common.h>

static pthread_t lvgl_tid;
static pthread_t lvgl_timer_tid;

static void* lvgl_entry(void *arg)
{
    int ret = -1;
    
    /* 初始化 lvgl */
    lv_init();

    /* 设备初始化 */
    lv_port_disp_init();
    lv_port_indev_init();
    lv_port_fs_init();

    /* app */

    while (1) 
    {
        /* 周期性调用 lv_timer_handler */
        lv_timer_handler();
        /* 官方建议是最少 5ms */
        usleep(5 * 1000);
    }
}

static void* lvgl_timer_entry(void *arg)
{
    while (1)
    {
        usleep(5 * 1000);
        /* tick 单位是 ms */
        lv_tick_inc(5);
    }
}

int main(void)
{
    int ret = -1;

    ret = pthread_create(&lvgl_tid, NULL, lvgl_entry, NULL);
    if (ret > 0)
        pthread_detach(lvgl_tid);

    /* 心跳由 SDL2 的 tick_thread 线程提供，这里先关闭 */
#if 0
    ret = pthread_create(&lvgl_timer_tid, NULL, lvgl_timer_entry, NULL);
    if (ret > 0)
        pthread_detach(lvgl_timer_tid);
#endif

    while(1);

    lv_port_disp_deinit();
    
    return 0;
}