// IOAccessのうちピンへのレベル設定に関してのサンプル

#include <IOAccess.hpp>
#include <esp32/rom/ets_sys.h>

const gpio_num_t OutputPort1 = GPIO_NUM_4;
const gpio_num_t OutputPort2 = GPIO_NUM_32;

extern "C" 
void app_main()
{
    gpio_pad_select_gpio(OutputPort1);
    gpio_set_direction(OutputPort1, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(OutputPort2);
    gpio_set_direction(OutputPort2, GPIO_MODE_OUTPUT);
    Take4::gpioClear(OutputPort1);
    Take4::gpioClear(OutputPort2);

    for (;;) {
        Take4::gpioSet(OutputPort1);
        Take4::gpioClear(OutputPort1);
        Take4::gpioSet(OutputPort2);
        Take4::gpioClear(OutputPort2);
    }
}