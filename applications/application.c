/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-04-29     supperthomas first version
 * 2021-06-26     supperthomas fix led
 *
 */

#define LOG_LVL LOG_LVL_DBG
#define LOG_TAG "main"
#include <ulog.h>

#include <rtthread.h>
#include <rtdevice.h>
#include <nrf52840.h>

int main(void)
{
//	uint32_t spim3_pri, spim2_pri;
//	spim3_pri = NVIC_GetPriority(SPIM3_IRQn);
//	spim2_pri = NVIC_GetPriority(SPIM2_SPIS2_SPI2_IRQn);
//	LOG_W("SPIM3 PRIO: %d", spim3_pri);
//	LOG_W("SPIM2 PRIO: %d", spim2_pri);
//	NVIC_SetPriority(SPIM2_SPIS2_SPI2_IRQn, 5);	
//	LOG_I("after set prio: %d", NVIC_GetPriority(SPIM2_SPIS2_SPI2_IRQn));
	
    int count = 1;

    while (count++)
    {
        rt_thread_mdelay(500);
    }
    return RT_EOK;
}


