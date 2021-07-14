#include "IRout.h"
#include <IRTime.h>
#if defined(IDF_VER)
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
namespace {
    uint32_t millis()
    {
        return esp_timer_get_time() / 1000;
    }
}
#endif
#if defined(ARDUINO)
#include <Arduino.h>
#endif

using namespace Take4;

#if defined(ARDUINO_ARCH_AVR)
// 1回分のデータを出力する
// out : 出力先となるポート
// pinMask : 出力先となるピン
// tTime : 変調単位(㎲)
// nT : 時間
// active : trueの場合アクティブ状態に保つ
void signalOut(volatile PortType_t* out, PortNo_t pinMask, uint32_t tTime, uint8_t nT, bool active)
{
	if (active) {
		// 当初 tTime*nT/26でループ回数を決め、for文で回す方向としたが、NECのリーダーの場合、規定タイム(8.9ms)ON状態にならなかった(8.5ms位)
		// ループ数は345回、8.5msで割ると、1回24.3㎲、もしかしたらdelayMicrosecondsは想定より若干少ないのかもしれない。そのため、累積誤差が大きくなり、時間が合わなくなったと思われる。
		// そこで、実時間経過による処理方法に変更した。この場合、micros()による計測時間は、1㎲以内だと仮定して実装をした。
		uint32_t start = micros();
		uint32_t end = start + (tTime * (uint32_t)nT) + 50;
		if (end < start) {
			while (micros() > start) {
				*out |= pinMask;
				delayMicroseconds(9);
				*out &= ~pinMask;
				delayMicroseconds(16);
			}
		}
		while (micros() < end) {
			*out |= pinMask;
			delayMicroseconds(9);
			*out &= ~pinMask;
			delayMicroseconds(16);
		}
	}
	else {
		delayMicroseconds(tTime * (uint32_t)nT - 50);
	}
}

// NEC/AEHA用データ送信
template<typename T>
void sendData1(T data, volatile PortType_t* out, PortNo_t pinMask, uint32_t tTime, uint8_t onT, uint8_t offT)
{
	T mask = 1;
	for (uint8_t i = 0; i < sizeof(T) * 8; i++, mask <<= 1) {
		signalOut(out, pinMask, tTime, 1, true);
		if (data & mask) {
			signalOut(out, pinMask, tTime, onT, false);
		}
		else {
			signalOut(out, pinMask, tTime, offT, false);
		}
	}
}

// SONY用のデータ送信
template<typename T>
void sendData2(T data, uint8_t nBit, volatile PortType_t* out, PortNo_t pinMask, uint32_t tTime, uint8_t onT, uint8_t offT)
{
	T mask = 1;
	for (uint8_t i = 0; i < nBit; i++, mask <<= 1) {
		signalOut(out, pinMask, tTime, 1, false);
		if (data & mask) {
			signalOut(out, pinMask, tTime, onT, true);
		}
		else {
			signalOut(out, pinMask, tTime, offT, true);
		}
	}
}

IRout::IRout()
: mask_(0)
, port_(0)
, out_(0)
{
}

void IRout::begin(PortNo_t pin)
{
	mask_ = digitalPinToBitMask(pin);
	port_ = digitalPinToPort(pin);
	out_ = portOutputRegister(port_);
	*portModeRegister(port_) |= mask_;
}

void IRout::send(const NECData & data)
{
	timer_ = waitTime(timer_, 108);
	// ヘッダー出力
	signalOut(out_, mask_, T1NECFormat, 16, true);
	if (data.formatType == NECFormat) {
		signalOut(out_, mask_, T1NECFormat, 8, false);
		sendData1(data.customerCode, out_, mask_, T1NECFormat, 3, 1);
		sendData1(data.data, out_, mask_, T1NECFormat, 3, 1);
		sendData1(~data.data, out_, mask_, T1NECFormat, 3, 1);
	}
	else {
		signalOut(out_, mask_, T1NECFormat, 4, false);
	}
	signalOut(out_, mask_, T1NECFormat, 1, true);
}

void IRout::send(const AEHAData & data)
{
	timer_ = waitTime(timer_, 130);
	// ヘッダー出力
	signalOut(out_, mask_, T1AEHAFormat, 8, true);
	if (data.formatType == AEHAFormat) {
		signalOut(out_, mask_, T1AEHAFormat, 4, false);
		sendData1(data.customerCode, out_, mask_, T1AEHAFormat, 3, 1);
		sendData1(data.parityData0, out_, mask_, T1AEHAFormat, 3, 1);
		for (uint8_t i = 0; i < data.nData; i++) {
			sendData1(data.data[i], out_, mask_, T1AEHAFormat, 3, 1);
		}
		delay(8);	// Trailer(8ms)
	}
	else {
		signalOut(out_, mask_, T1AEHAFormat, 8, false);
	}
	signalOut(out_, mask_, T1AEHAFormat, 1, true);
}

void IRout::send(const SONYData & data)
{
	timer_ = waitTime(timer_, 45);
	// ヘッダー出力
	signalOut(out_, mask_, T1SONYFormat, 4, true);
	sendData2(data.data, 7, out_, mask_, T1SONYFormat, 2, 1);
	sendData2(data.address, data.nAddress, out_, mask_, T1SONYFormat, 2, 1);
}
#endif // ARDUINO_ARCH_AVR

uint32_t IRout::waitTime(uint32_t startTime, uint32_t waitTime)
{
	if (startTime > 0) {
		uint32_t remain = millis() - startTime;
		remain -= waitTime;
		if (remain < waitTime) {
#if defined(IDF_VER)
			vTaskDelay(remain / portTICK_PERIOD_MS);
#else
			delay(remain);
#endif
		}
	}
	return millis();
}

void IRout::send(const IRData & data)
{
	switch (data.formatType) {
	case NECFormat:
	case NECFormatRepeat:
		send(data.nec);
		break;
	case SONYFormat:
	case SONYFormatRepeat:
		send(data.sony);
		break;
	case AEHAFormat:
	case AEHAFormatRepeat:
		send(data.aeha);
		break;
	default:
		break;
	}
}
