/*
 * @Author       : Zeepunt.H
 * @Date         : 2022-03-12 17:07:44
 * @LastEditTime : 2022-03-22 17:37:10
 * Gitee : https://gitee.com/zeepunt
 * Github: https://github.com/Recaa
 *  
 * Copyright (c) 2022 by Zeepunt.H, All Rights Reserved. 
 */

#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>

enum
{
    LOG_DEBUG = 0,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_END = 0xFF,
};

#define LOG_LEVEL LOG_DEBUG

#define LOG printf

#define LOG_DEBUG(fmt, ...)														    	\
	do {																		    	\
        if (LOG_LEVEL <= LOG_DEBUG)                                                     \
		    LOG("\033[37m[%s:%d]" fmt "\033[0m\n", __func__, __LINE__, ##__VA_ARGS__);	\
	} while (0)

#define LOG_INFO(fmt, ...)														    	\
	do {																		   	 	\
        if (LOG_LEVEL <= LOG_INFO)                                                      \
		    LOG("\033[32m[%s:%d]" fmt "\033[0m\n", __func__, __LINE__, ##__VA_ARGS__);  \
	} while (0)

#define LOG_WARN(fmt, ...)														    	\
	do {																		    	\
        if (LOG_LEVEL <= LOG_WARN)                                                      \
		    LOG("\033[33m[%s:%d]" fmt "\033[0m\n", __func__, __LINE__, ##__VA_ARGS__);	\
	} while (0)

#define LOG_ERROR(fmt, ...)														    	\
	do {																		    	\
        if (LOG_LEVEL <= LOG_ERROR)                                                     \
		    LOG("\033[31m[%s:%d]" fmt "\033[0m\n", __func__, __LINE__, ##__VA_ARGS__);	\
	} while (0)

#endif