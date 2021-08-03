#pragma once
#include <driver/gpio.h>

namespace Take4
{
    // gpio_set_level(x, 1) 相当
    inline void gpioSet(gpio_num_t x)
    {
        if (x < GPIO_NUM_32)
            GPIO.out_w1ts = 1 << x;
        else
            GPIO.out1_w1ts.val = 1 << (x - GPIO_NUM_32);
    }

    // gpio_set_level(x, 0) 相当
    inline void gpioClear(gpio_num_t x)
    {
        if (x < GPIO_NUM_32)
            GPIO.out_w1tc = 1 << x;
        else
            GPIO.out1_w1tc.val = 1 << (x - GPIO_NUM_32);
    }

    // gpio_get_level(x) 相当
    // return : Lowの場合0、Highは!=0
    inline uint32_t gpioRead(gpio_num_t x)
    {
        if (x < GPIO_NUM_32)
            return GPIO.in & (1 << x);
        else
            return GPIO.in1.val & (1 << (x - GPIO_NUM_32));
    }

} // namespace Take4
