#if defined(ARDUINO)
// Arduinoプラットフォーム用サンプルプログラム
#include "Magnetics.h"
Take4::Magnetics c1;

#if defined(ESP_PLATFORM)
const Port_t port1 = ADC1_CHANNEL_4;
const Port_t port2 = ADC1_CHANNEL_5;
const Port_t port3 = ADC1_CHANNEL_6;
const Port_t port4 = ADC1_CHANNEL_7;
#endif
#if defined(ARDUINO_AVR_UNO)
const Port_t port1 = 0;
const Port_t port2 = 1;
const Port_t port3 = 2;
const Port_t port4 = 3;
#endif

void setup()
{
	Serial.begin(115200);
	delay(100);

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
			Serial.print(c1.get(i));
			Serial.print(",");
		}
		printf("%d,", c1.get(i));
	}
}
#endif