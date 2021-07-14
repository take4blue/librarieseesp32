#if !defined(ARDUINO)
// ESP-IFプラットフォーム用サンプルプログラム
#include "Magnetics.h"
Take4::Magnetics c1;

const Port_t port1 = ADC1_CHANNEL_4;
const Port_t port2 = ADC1_CHANNEL_5;
const Port_t port3 = ADC1_CHANNEL_6;
const Port_t port4 = ADC1_CHANNEL_7;

void setup()
{
	c1.add(port1);
	c1.add(port2);
	//c1.add(port3);
	//c1.add(port4);
	c1.begin();
}

void loop()
{
	if (c1.check()) {
		int i = 0;
		for (; i < c1.nPort() - 1; ++i) {
			printf("%d,", c1.get(i));
		}
		printf("%d\n", c1.get(i));
	}
}

extern "C"
void app_main()
{
	setup();
	for(;;) {
		loop();
	}
}
#endif