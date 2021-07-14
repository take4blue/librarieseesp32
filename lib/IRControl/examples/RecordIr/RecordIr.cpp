#if defined(ARDUINO_ARCH_AVR)
#include "RecordIrTop.h"

const PortNo_t IRSEND_PIN = 3;   // IRの受信ピン
RecordIr::App<8> app_;

void setup() {
    app_.begin(IRSEND_PIN);
}
void loop() {
    app_.task();
}
#endif