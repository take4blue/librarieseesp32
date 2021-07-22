#pragma once
#include <IRCommon.h>

namespace Take4
{
// 赤外線リモコンデータ出力
// delayのためESP_PLATFORMの場合1tick1msが望ましい。
// ARDUINO_ARCH_AVR, ESP_PLATFORM
// 送信データは IRCommon.h ないで定義されているNEC/AEHA/SONYの各フォーマットを使用すること
class IRout
{
private:
	// 出力先のピン情報
#if defined(ARDUINO_ARCH_AVR)
	PortNo_t mask_;
	PortType_t port_;
	volatile PortType_t *out_;
#elif defined(ESP_PLATFORM)
	uint8_t channel_;
#endif

	// Keyリピートのための時間情報(㎲)
	uint32_t timer_;

	static uint32_t waitTime(uint32_t startTime, uint32_t waitTime);

public:
	IRout();

	// 初期化処置
	// pin : ioポート番号
	// channel : RMTのチャンネル番号
	void begin(PortNo_t pin
#if defined(ESP_PLATFORM)
        , uint8_t channel
#endif
	);

	// データの送信
	// data : 送信データ
	void send(const NECData& data);

	// データの送信
	// data : 送信データ
	void send(const SONYData& data);

	// データの送信
	// data : 送信データ
	void send(const AEHAData& data);

	// データの送信
	// data : 送信データ
	void send(const IRData& data);
};
}