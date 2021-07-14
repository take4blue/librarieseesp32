// Arduinoベースでのサンプルプログラム
#if defined(ARDUINO)
#include <IRin.h>
#include <Arduino.h>
#if defined(ARDUINO_ARCH_AVR)
const PortNo_t IRSEND_PIN = 3;   // IRの受信ピン
#elif defined(ESP_PLATFORM)
const PortNo_t IRSEND_PIN = 3;   // IRの受信ピン
#endif

Take4::IRin ii;

void setup() {
  Serial.begin(9600);

  pinMode(IRSEND_PIN, INPUT);
  ii.begin(IRSEND_PIN
#if defined(ESP_PLATFORM)
    , 0
#endif
  );
  Serial.println("Start");
}

void loop() {
  if (ii.parse()) {
    switch (ii.formatType()) {
      case Take4::NECFormat:
        Serial.print("NEC ");
        Serial.print(ii.data().nec.customerCode, HEX);
        Serial.print(" ");
        Serial.println(ii.data().nec.data, HEX);
        break;
      case Take4::AEHAFormat:
        Serial.print("AEHA ");
        Serial.print(ii.data().aeha.customerCode, HEX);
        Serial.print(" ");
        Serial.print(ii.data().aeha.parityData0, HEX);
        Serial.print(" ");
        for (unsigned int i = 0; i < ii.data().aeha.nData; i++) {
          Serial.print(ii.data().aeha.data[i], HEX);
          Serial.print(" ");
        }
        Serial.println("");
        break;
      case Take4::SONYFormat:
        Serial.print("SONY ");
        Serial.print(ii.data().sony.data, HEX);
        Serial.print(" ");
        Serial.println(ii.data().sony.address, HEX);
        break;
      case Take4::NOFormat:
        Serial.println("OFF");
        break;
      default:
        break;
    }
    if (ii.isRepeat()) {
      Serial.println("Repeat");
    }
  }
}
#endif