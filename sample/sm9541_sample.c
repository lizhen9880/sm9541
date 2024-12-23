#include <rtthread.h>
#include <rtdevice.h>
#include <rtdef.h>
#include "sm95_device.h"

#define DBG_TAG "sm9541.sample"
#ifdef RT_I2C_DEBUG
#define DBG_LVL DBG_LOG
#else
#define DBG_LVL DBG_INFO
#endif

#include <rtdbg.h>
static void sm9541_read_entry(void *parameter)
{
    rt_device_t sm95_dev = RT_NULL;
    struct rt_sensor_data sm95_data[2];
    rt_size_t res95 = 0;

    rt_int32_t sm9541_p, sm9541_t; /* transform buf */

    /* find have been registered device*/
    sm95_dev = rt_device_find("baro_sm9541_sensor");
    if (sm95_dev == RT_NULL)
    {
        LOG_E("not found sm9541 device!\r\n");
        return;
    }
    else
    {
        LOG_I("found sm9541 device! \r\n");
    }

    /* open found sensor */
    if (rt_device_open(sm95_dev, RT_DEVICE_FLAG_RDWR) != RT_EOK)
    {
        LOG_E("open sm9541 device failed! \r\n");
        return;
    }
    else
    {
        LOG_I("open sm9541 device succeed! \r\n");
    }

    while (1)
    {
        /* sm9541 read data */
        res95 = rt_device_read(sm95_dev, 0, &sm95_data, 2);
        if (res95 == 0)
        {
            LOG_E("sm9541 read data failed! result is %d \n", res95);
            rt_device_close(sm95_dev);
            return;
        }
        else
        {
            sm9541_p = sm95_data[0].data.baro;
            sm9541_t = sm95_data[1].data.temp;
            LOG_I("sm9541 fetch pressure is %d.%d, %d\r\n",
                  sm9541_p / SM95_BARO_X_N, sm9541_p > 0 ? sm9541_p % SM95_BARO_X_N : -sm9541_p % SM95_BARO_X_N,
                  sm95_data[0].timestamp);
            LOG_I("sm9541 fetch temperature is %d.%d, %d\r\n",
                  sm9541_t / SM95_TEMP_X_N, sm9541_t > 0 ? sm9541_t % SM95_TEMP_X_N : -sm9541_t % SM95_TEMP_X_N,
                  sm95_data[1].timestamp);
        }
        rt_thread_delay(500);
    }
}

static int sm9541_read_sample(void)
{
    rt_thread_t sm9541_thread;

    sm9541_thread = rt_thread_create("sm95_tid",
                                     sm9541_read_entry,
                                     RT_NULL,
                                     1024,
                                     RT_THREAD_PRIORITY_MAX / 2,
                                     20);
    if (sm9541_thread != RT_NULL)
    {
        rt_thread_startup(sm9541_thread);
        rt_kprintf("sm9541 thread is running! \r\n");
    }

    return RT_EOK;
}
MSH_CMD_EXPORT(sm9541_read_sample, start sm9541 read sample);

#define THREAD_PRIORITY 25
#define THREAD_STACK_SIZE 512
#define THREAD_TIMESLICE 5
static char sm95_thread_stack[1024];
static struct rt_thread sm95_thread;
static int sm9541_read_sample_static(void)
{
    rt_thread_init(&sm95_thread,
                   "thread2",
                   sm9541_read_entry,
                   RT_NULL,
                   &sm95_thread_stack[0],
                   sizeof(sm95_thread_stack),
                   THREAD_PRIORITY - 1, THREAD_TIMESLICE);
    rt_thread_startup(&sm95_thread);
    rt_kprintf("sm9541 thread(static) is running! \r\n");
    return RT_EOK;
}
MSH_CMD_EXPORT(sm9541_read_sample_static, start sm9541 read sample);