#if defined(ESP_PLATFORM)
#include "IRTime.h"
#if defined(ARDUINO)
#include <Arduino.h>
#endif
#include <IRin.h>
#include <driver/rmt.h>

using namespace Take4;

IRin::IRin()
    : channel_(0)
    , ringBuffer_(nullptr)
{
}

void IRin::begin(PortNo_t pin, uint8_t channel)
{
    channel_ = channel;
    rmt_config_t rmt_rx;
    rmt_rx.rmt_mode = RMT_MODE_RX;
    rmt_rx.channel = (rmt_channel_t)channel_;
    rmt_rx.gpio_num = (gpio_num_t)pin;
    rmt_rx.clk_div = 80; // 1μ秒を基準単位にする
    rmt_rx.mem_block_num = 1;
    rmt_rx.rx_config.filter_en = true;
    rmt_rx.rx_config.filter_ticks_thresh = 100;
    // アイドルを図るための閾値の設定
    // NECのリーダーを受け入れるため10msにしておく
    rmt_rx.rx_config.idle_threshold = 10000 / 10 * (80000000 / rmt_rx.clk_div / 100000); // 10000 / 10 * 10μ秒相当のtickカウント数(一応4ミリ秒)
    rmt_config(&rmt_rx);
    rmt_driver_install(rmt_rx.channel, 1000, 0); // リングバッファのサイズは最大でAEHAフォーマットのdata4まで読み込みで57ビット分
    rmt_get_ringbuf_handle(rmt_rx.channel, &ringBuffer_);
    rmt_rx_start(rmt_rx.channel, true);
}

IRin::~IRin()
{
    rmt_rx_stop((rmt_channel_t)channel_);
    rmt_driver_uninstall((rmt_channel_t)channel_);
}

// SONY形式データの解析(Highの長さで1,0判断)
static void analyzeSony(IRData& data, const rmt_item32_t* item, size_t size)
{
    data.sony.data = 0;
    data.sony.address = 0;
    size_t i = 1;
    for (size_t j = 0; i < size && j < 7; i++, j++) {
        if (item[i].level0 == 0 && item[i].duration0 > T1SONYFormat * 1.7) {
            data.sony.data |= 1 << j;
        }
    }
    for (size_t j = 0; i < size && j < 13; i++, j++) {
        if (item[i].level0 == 0 && item[i].duration0 > T1SONYFormat * 1.7) {
            data.sony.address |= 1 << j;
        }
    }
	data.sony.nAddress = size - 8;
}

// NEC, AEHAフォーマットのデータ取り出しコード
// Highの時の長さにより1,0を決定する
// 0は1T、1は3Tだが、識別条件は2.5Tにしている
static unsigned long getDataH(const rmt_item32_t* item, size_t size, size_t nBit, unsigned int dt)
{
    unsigned long value = 0;
    for (size_t i = 0; i < nBit && i < size; i++) {
        if (item[i].level1 == 1 && item[i].duration1 > dt * 2.5) {
            value |= 1 << i;
        }
    }
    return value;
}

bool IRin::parse()
{
    if (ringBuffer_) {
        size_t rx_size = 0;
        rmt_item32_t* item = (rmt_item32_t*)xRingbufferReceive(ringBuffer_, &rx_size, 300);
        if (item) {
            IRFormatType wFormat = data_.formatType;
            if (rx_size > sizeof(rmt_item32_t) && item[0].level0 == 0) {
                // リングバッファにあるデータを解析
                // 一つ目のデータをヘッダー情報として認識し、フォーマットを判別する
                // フォーマット検出
                auto size = rx_size / sizeof(rmt_item32_t);
                if (item[0].duration0 > TFrameLeadLNEC) {
                    if (item[0].duration1 > TFrameLeadHNEC) {
                        if (size >= 33) {
                            data_.nec.customerCode = getDataH(item + 1, size - 1, 16, T1NECFormat);
                            data_.nec.data = getDataH(item + 17, size - 17, 16, T1NECFormat);
                        } else {
                            data_.formatType = NOFormat;
                        }
                        if (wFormat == NECFormat || wFormat == NECFormatRepeat) {
                            data_.formatType = NECFormatRepeat;
                        } else {
                            data_.formatType = NECFormat;
                        }
                    } else if (item[0].duration1 > TFrameRepeatHNEC) {
                        data_.formatType = NECFormatRepeat;
                    } else {
                        data_.formatType = NOFormat;
                    }
                } else if (item[0].duration0 > TFrameLeadLAHEA) {
					if (item[0].duration1 > TFrameRepeatHAHEA) {
						data_.formatType = AEHAFormatRepeat;
					}
                    else if (item[0].duration1 > TFrameLeadHAHEA) {
                        if (size >= 25) {
                            data_.aeha.customerCode = getDataH(item + 1, size - 1, 16, T1AEHAFormat);
                            data_.aeha.parityData0 = getDataH(item + 17, size - 17, 8, T1AEHAFormat);
                            data_.aeha.nData = (size - 25) / 8;
                            for (size_t i = 0; i < data_.aeha.nData; i++) {
                                data_.aeha.data[i] = getDataH(item + 25 + i * 8, size - (25 + i * 8), 8, T1AEHAFormat);
                            }
                            if (wFormat == AEHAFormat || wFormat == AEHAFormatRepeat) {
                                data_.formatType = AEHAFormatRepeat;
                            } else {
                                data_.formatType = AEHAFormat;
                            }
                        } else {
                            data_.formatType = NOFormat;
                        }
                    } else {
                        data_.formatType = NOFormat;
                    }
                } else if (item[0].duration0 > TFrameLeadLSONY) {
                    if (wFormat == SONYFormat || wFormat == SONYFormatRepeat) {
                        data_.formatType = SONYFormatRepeat;
                    } else {
                        data_.formatType = SONYFormat;
                    }
                    analyzeSony(data_, item, size);
                } else {
                    data_.formatType = NOFormat;
                }
            }
            else {
                data_.formatType = NOFormat;
            }
            //after parsing the data, return spaces to ringbuffer.
            vRingbufferReturnItem(ringBuffer_, (void*)item);
            return (wFormat == NOFormat && data_.formatType == NOFormat) ? false : true;
        }
        data_.formatType = NOFormat;
    }
    return false;
}

#endif