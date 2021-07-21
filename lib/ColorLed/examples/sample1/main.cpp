#include "Color2LedControl.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

const gpio_num_t pin = GPIO_NUM_32;
const rmt_channel_t channel = RMT_CHANNEL_0;
const size_t NumLed = 2;

Take4::Color2LedControl led;

extern "C"
void app_main()
{
    Take4::RGBBuffer_t counter = 0xC8;
    Take4::RGBBuffer_t rgb[NumLed * Take4::Color2LedControl::BufferBlockSize];
    led.begin(pin, channel);

    for (;;counter += 10) {
        rgb[0] = rgb[1] = rgb[2] = counter;
        rgb[3] = rgb[4] = rgb[5] = counter + 128;
        led.update(rgb, NumLed);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}