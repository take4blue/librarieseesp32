#if defined(ESP_PLATFORM)
#if defined(ARDUINO)
#include <Arduino.h>
#endif
#if defined(IDF_VER)
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#endif
#include "IRout.h"
#include <IRTime.h>
#include <driver/rmt.h>

using namespace Take4;

// RmtItem32を効率よく管理するためのクラス
// コンストラクタで、必要数確保する。
struct RmtItem32 {
	rmt_item32_t* item_;
	uint16_t maxItem_;
	uint16_t pos_;

	RmtItem32(uint16_t maxItem)
		: item_(nullptr)
		, maxItem_(maxItem)
		, pos_(0)
	{
		item_ = (rmt_item32_t*)malloc(sizeof(rmt_item32_t) * maxItem_);
		if (item_) {
			memset(item_, 0, sizeof(rmt_item32_t) * maxItem_);
		}
	}

	~RmtItem32()
	{
		if (item_) {
			free(item_);
		}
	}

	// このメソッドでは、levelとdurationという2つだけの構造体に対してデータを設定する
	// という考え方になっている。
	// これは、SONY方式がリーダーの後すぐにデータ送信が始まり、rmt_item32_tをそのまま
	// 利用するには、取扱いづらいため
	void push(uint8_t level, uint32_t duration)
	{
		if (item_ && (pos_ / 2) < maxItem_) {
			if ((pos_ % 2) == 0) {
				item_[pos_ / 2].level0 = level;
				item_[pos_ / 2].duration0 = duration;
			}
			else {
				item_[pos_ / 2].level1 = level;
				item_[pos_ / 2].duration1 = duration;
			}
			pos_++;
		}
	}

	// データの送信処理
	void send(rmt_channel_t channel)
	{
		if (item_) {
			rmt_write_items(channel, item_, maxItem_, true);
			rmt_wait_tx_done(channel, portMAX_DELAY);
		}
	}
};

IRout::IRout()
	: channel_(0)
{
}

void IRout::begin(PortNo_t pin, uint8_t channel)
{
	channel_ = channel;
	rmt_config_t rmt_tx;
	rmt_tx.channel = (rmt_channel_t)channel_;
	rmt_tx.gpio_num = (gpio_num_t)pin;
	rmt_tx.mem_block_num = 1;
	rmt_tx.clk_div = 80; // 1μ秒を基準単位にする
	rmt_tx.tx_config.loop_en = false;
	rmt_tx.tx_config.carrier_duty_percent = 33;
	rmt_tx.tx_config.carrier_freq_hz = 38000;
	rmt_tx.tx_config.carrier_level = RMT_CARRIER_LEVEL_HIGH;
	rmt_tx.tx_config.carrier_en = true;
	rmt_tx.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
	rmt_tx.tx_config.idle_output_en = true;
	rmt_tx.rmt_mode = RMT_MODE_TX;
	rmt_config(&rmt_tx);
	rmt_driver_install(rmt_tx.channel, 0, 0);
}

// NEC, AEHA用のデータ設定処理
template<typename T>
void sendData1(T data, RmtItem32& work, uint32_t tTime, uint8_t onT, uint8_t offT)
{
	T mask = 1;
	for (uint8_t i = 0; i < sizeof(T) * 8; i++, mask <<= 1) {
		work.push(1, tTime);
		if (data & mask) {
			work.push(0, tTime * onT);
		}
		else {
			work.push(0, tTime * offT);
		}
	}
}

// SONY用のデータ設定処理
template<typename T>
void sendData2(T data, uint8_t nBit, RmtItem32& work, uint32_t tTime, uint8_t onT, uint8_t offT)
{
	T mask = 1;
	for (uint8_t i = 0; i < nBit; i++, mask <<= 1) {
		work.push(0, tTime);
		if (data & mask) {
			work.push(1, tTime * onT);
		}
		else {
			work.push(1, tTime * offT);
		}
	}
}

void IRout::send(const NECData & data)
{
	timer_ = waitTime(timer_, 108);
	// ヘッダー出力
	if (data.formatType == NECFormat) {
		RmtItem32 work(1 + 16 + 16 + 1);
		work.push(1, T1NECFormat * 16);
		work.push(0, T1NECFormat * 8);
		sendData1(data.customerCode, work, T1NECFormat, 3, 1);
		sendData1(data.data, work, T1NECFormat, 3, 1);
		sendData1(~data.data, work, T1NECFormat, 3, 1);
		work.push(1, T1NECFormat);
		work.push(0, T1NECFormat);
		work.send((rmt_channel_t)channel_);
	}
	else {
		RmtItem32 work(2);
		work.push(1, T1NECFormat * 16);
		work.push(0, T1NECFormat * 4);
		work.push(1, T1NECFormat);
		work.push(0, T1NECFormat);
		work.send((rmt_channel_t)channel_);
	}
}

void IRout::send(const AEHAData & data)
{
	timer_ = waitTime(timer_, 130);
	// ヘッダー出力
	if (data.formatType == AEHAFormat) {
		RmtItem32 work(1 + 16 + 8 + 8 * data.nData + 1);
		work.push(1, T1AEHAFormat * 8);
		work.push(0, T1AEHAFormat * 4);
		sendData1(data.customerCode, work, T1AEHAFormat, 3, 1);
		sendData1(data.parityData0, work, T1AEHAFormat, 3, 1);
		for (uint8_t i = 0; i < data.nData; i++) {
			sendData1(data.data[i], work, T1AEHAFormat, 3, 1);
		}
		work.push(1, T1AEHAFormat);
		work.push(0, T1AEHAFormat);
		work.send((rmt_channel_t)channel_);
	}
	else {
		RmtItem32 work(2);
		work.push(1, T1AEHAFormat * 8);
		work.push(0, T1AEHAFormat * 8);
		work.push(1, T1AEHAFormat);
		work.push(0, T1AEHAFormat);
		work.send((rmt_channel_t)channel_);
	}
#if defined(IDF_VER)
	vTaskDelay(8 / portTICK_PERIOD_MS);
#else
	delay(8);	// Trailer(8ms)
#endif
}

void IRout::send(const SONYData & data)
{
	timer_ = waitTime(timer_, 45);
	// ヘッダー出力
	RmtItem32 work(1 + 14 + data.nAddress * 2);
	work.push(1, T1SONYFormat * 4);
	sendData2(data.data, 7, work, T1SONYFormat, 2, 1);
	sendData2(data.address, data.nAddress, work, T1SONYFormat, 2, 1);
	work.send((rmt_channel_t)channel_);
}

#endif // !ESP32
