/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2012-01-29     aozima       first version.
 * 2012-04-12     aozima       optimization: find rtc device only first.
 * 2012-04-16     aozima       add scheduler lock for set_date and set_time.
 * 2018-02-16     armink       add auto sync time by NTP
 * 2021-05-09     Meco Man     remove NTP
 * 2021-06-11     iysheng      implement RTC framework V2.0
 * 2021-07-30     Meco Man     move rtc_core.c to rtc.c
 */
#define LOG_LVL LOG_LVL_DBG
#define LOG_TAG "rtc"
#include <ulog.h>

#include <string.h>
#include <stdlib.h>
#include <rtthread.h>
#include <drivers/rtc.h>

#include "t_comm.h"

#ifdef RT_USING_RTC

static rt_device_t _rtc_device;

/**
 * Set system date(time not modify, local timezone).
 *
 * @param rt_uint32_t year  e.g: 2012.
 * @param rt_uint32_t month e.g: 12 (1~12).
 * @param rt_uint32_t day   e.g: 31.
 *
 * @return rt_err_t if set success, return RT_EOK.
 */
rt_err_t set_date(rt_uint32_t year, rt_uint32_t month, rt_uint32_t day)
{
    time_t now, old_timestamp = 0;
    struct tm tm_new = {0};
    rt_err_t ret = -RT_ERROR;

    if (_rtc_device == RT_NULL)
    {
        _rtc_device = rt_device_find("rtc");
        if (_rtc_device == RT_NULL)
        {
            return -RT_ERROR;
        }
    }

    /* get current time */
    ret = rt_device_control(_rtc_device, RT_DEVICE_CTRL_RTC_GET_TIME, &old_timestamp);
    if (ret != RT_EOK)
    {
        return ret;
    }

    /* converts calendar time into local time. */
    localtime_r(&old_timestamp, &tm_new);

    /* update date. */
    tm_new.tm_year = year - 1900;
    tm_new.tm_mon  = month - 1; /* tm_mon: 0~11 */
    tm_new.tm_mday = day;

    /* converts the local time into the calendar time. */
    now = mktime(&tm_new);

    /* update to RTC device. */
    ret = rt_device_control(_rtc_device, RT_DEVICE_CTRL_RTC_SET_TIME, &now);
    return ret;
}

/**
 * Set system time(date not modify, local timezone).
 *
 * @param rt_uint32_t hour   e.g: 0~23.
 * @param rt_uint32_t minute e.g: 0~59.
 * @param rt_uint32_t second e.g: 0~59.
 *
 * @return rt_err_t if set success, return RT_EOK.
 */
rt_err_t set_time(rt_uint32_t hour, rt_uint32_t minute, rt_uint32_t second)
{
    time_t now, old_timestamp = 0;
    struct tm tm_new = {0};
    rt_err_t ret = -RT_ERROR;

    if (_rtc_device == RT_NULL)
    {
        _rtc_device = rt_device_find("rtc");
        if (_rtc_device == RT_NULL)
        {
            return -RT_ERROR;
        }
    }

    /* get current time */
    ret = rt_device_control(_rtc_device, RT_DEVICE_CTRL_RTC_GET_TIME, &old_timestamp);
    if (ret != RT_EOK)
    {
        return ret;
    }

    /* converts calendar time into local time. */
    localtime_r(&old_timestamp, &tm_new);

    /* update time. */
    tm_new.tm_hour = hour;
    tm_new.tm_min  = minute;
    tm_new.tm_sec  = second;

    /* converts the local time into the calendar time. */
    now = mktime(&tm_new);

    /* update to RTC device. */
    ret = rt_device_control(_rtc_device, RT_DEVICE_CTRL_RTC_SET_TIME, &now);
    return ret;
}

/**
 * Set timestamp(UTC).
 *
 * @param time_t timestamp
 *
 * @return rt_err_t if set success, return RT_EOK.
 */
rt_err_t set_timestamp(time_t timestamp)
{
    if (_rtc_device == RT_NULL)
    {
        _rtc_device = rt_device_find("rtc");
        if (_rtc_device == RT_NULL)
        {
            return -RT_ERROR;
        }
    }

    /* update to RTC device. */
    return rt_device_control(_rtc_device, RT_DEVICE_CTRL_RTC_SET_TIME, &timestamp);
}

/**
 * Get timestamp(UTC).
 *
 * @param time_t* timestamp
 *
 * @return rt_err_t if set success, return RT_EOK.
 */
rt_err_t get_timestamp(time_t *timestamp)
{
    if (_rtc_device == RT_NULL)
    {
        _rtc_device = rt_device_find("rtc");
        if (_rtc_device == RT_NULL)
        {
            return -RT_ERROR;
        }
    }

    /* Get timestamp from RTC device. */
    return rt_device_control(_rtc_device, RT_DEVICE_CTRL_RTC_GET_TIME, timestamp);
}

#ifdef RT_USING_FINSH
#include <finsh.h>
/**
 * get date and time or set (local timezone) [year month day hour min sec]
 */
static void date(int argc, char **argv)
{
    time_t now = (time_t)0;

    if (argc == 1)
    {
		get_timestamp(&now);
		rt_kprintf("get time sample: %d\r\n", now);
		rt_kprintf("local time: %.*s", 25U, ctime(&now));
    }
    else if (argc >= 7)
    {
        /* set time and date */
        struct tm tm_new = {0};
        time_t old = (time_t)0;
        rt_err_t err;

        tm_new.tm_year = atoi(argv[1]) - 1900;
        tm_new.tm_mon = atoi(argv[2]) - 1; /* .tm_min's range is [0-11] */
        tm_new.tm_mday = atoi(argv[3]);
        tm_new.tm_hour = atoi(argv[4]);
        tm_new.tm_min = atoi(argv[5]);
        tm_new.tm_sec = atoi(argv[6]);
        if (tm_new.tm_year <= 0)
        {
            rt_kprintf("year is out of range [1900-]\n");
            return;
        }
        if (tm_new.tm_mon > 11) /* .tm_min's range is [0-11] */
        {
            rt_kprintf("month is out of range [1-12]\n");
            return;
        }
        if (tm_new.tm_mday == 0 || tm_new.tm_mday > 31)
        {
            rt_kprintf("day is out of range [1-31]\n");
            return;
        }
        if (tm_new.tm_hour > 23)
        {
            rt_kprintf("hour is out of range [0-23]\n");
            return;
        }
        if (tm_new.tm_min > 59)
        {
            rt_kprintf("minute is out of range [0-59]\n");
            return;
        }
        if (tm_new.tm_sec > 60)
        {
            rt_kprintf("second is out of range [0-60]\n");
            return;
        }
        /* save old timestamp */
        err = get_timestamp(&old);
        if (err != RT_EOK)
        {
            rt_kprintf("Get current timestamp failed. %d\n", err);
            return;
        }
        /* converts the local time into the calendar time. */
        now = mktime(&tm_new);
        err = set_timestamp(now);
        if (err != RT_EOK)
        {
            rt_kprintf("set date failed. %d\n", err);
            return;
        }
        get_timestamp(&now); /* get new timestamp */
        rt_kprintf("old: %.*s", 25, ctime(&old));
        rt_kprintf("now: %.*s", 25, ctime(&now));
    }
    else
    {
        rt_kprintf("please input: date [year month day hour min sec] or date\n");
        rt_kprintf("e.g: date 2018 01 01 23 59 59 or date\n");
    }
}
MSH_CMD_EXPORT(date, get date and time or set (local timezone) [year month day hour min sec])
	

/* 获取当前时间 */
time_t get_date()
{
	time_t now = (time_t)0;
	get_timestamp(&now);
	rt_kprintf("now: %d\n", now);
	rt_kprintf("now: %.*s\n", 25, ctime(&now));
	return now;
}
MSH_CMD_EXPORT(get_date, get data)


/* 获取当前时间 */
static void set_date_entry(void* parameter)
{
	time_t now;
	rt_err_t err;
	while(1)
	{
		/* 接收p_context */
		if(RT_EOK == rt_mb_recv(&set_date_mb, (rt_ubase_t *)&now, RT_WAITING_FOREVER))
		{
			err = set_timestamp(now);
			if (err != RT_EOK)
			{
				LOG_E("set date failed. %d\n", err);
			}
			else
			{
				LOG_W("set timestamp: %d", now);
			}
		}
	}
	
}

#define SET_DATE_THREAD_PRIORITY RT_MAIN_THREAD_PRIORITY+3
#define SET_DATE_THREAD_STACK_SIZE 1024
#define SET_DATE_THREAD_TIMESLICE 10

static rt_uint8_t t_set_date_thread_stack[SET_DATE_THREAD_STACK_SIZE];
static struct rt_thread t_set_date_thread;

int t_set_date_thread_init(void)
{
	//thread init
	rt_thread_init(&t_set_date_thread, "t_set_date", set_date_entry, RT_NULL, &t_set_date_thread_stack[0], SET_DATE_THREAD_STACK_SIZE, SET_DATE_THREAD_PRIORITY, SET_DATE_THREAD_TIMESLICE);
	rt_thread_startup(&t_set_date_thread);
	
	return 0;
}

INIT_APP_EXPORT(t_set_date_thread_init);
	

#endif /* RT_USING_FINSH */

#endif /* RT_USING_RTC */
