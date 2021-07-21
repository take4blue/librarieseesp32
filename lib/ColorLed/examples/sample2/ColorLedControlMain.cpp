#include "ColorLedControl.h"

const gpio_num_t pin = GPIO_NUM_32;
const rmt_channel_t channel = RMT_CHANNEL_0;
const size_t NumLed = 3;

Take4::ColorLedControl ledControler(10);

void colorSet(int i, size_t no, Take4::ColorLedControl& led)
{
    switch (i % 8)
    {
    case 0:
        led.set(no, 0, 0, 0);
        break;
    
    case 1:
        led.set(no, 0xFF, 0, 0);
        break;

    case 2:
        led.set(no, 0, 0xFF, 0);
        break;
        
    case 3:
        led.set(no, 0, 0, 0xFF);
        break;
        
    case 4:
        led.set(no, 0xFF, 0xFF, 0xFF);
        break;
        
    case 5:
        led.set(no, 0, 0xFF, 0xFF);
        break;

    case 6:
        led.set(no, 0xFF, 0, 0xFF);
        break;
        
    case 7:
        led.set(no, 0xFF, 0xFF, 0);
        break;

    default:
        break;
    }
}

extern "C"
void app_main()
{
    ledControler.begin(pin, channel);

    for (size_t counter = 0;;++counter) {
        colorSet(counter + 0, 0, ledControler);
        colorSet(counter + 1, 1, ledControler);
        colorSet(counter + 2, 2, ledControler);
        ledControler.wait(false);
        ledControler.update();
    }
}