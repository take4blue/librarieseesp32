#include "IRStrobe.h"

using namespace Take4;

static constexpr size_t PatternBufferSize = 20;
static constexpr size_t ValueBitPosition = 15;
static constexpr uint32_t HighDuration = 60;
static constexpr uint32_t LowDuration = 40;

IRStrobe::IRStrobe()
: channel_(RMT_CHANNEL_0)
, value_(0)
, type_(StrobeType::Manual)
, igniteStrobe_(false)
{    
}

IRStrobe::~IRStrobe()
{    
}

// 初期化関数
void IRStrobe::begin(gpio_num_t pin, rmt_channel_t channel)
{
    channel_ = channel;
    rmt_config_t config = RMT_DEFAULT_CONFIG_TX(pin, channel_);
    config.tx_config.carrier_en = false;
    config.clk_div = 80;

    ESP_ERROR_CHECK(rmt_config(&config));
    ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));
}

// 光量の設定
void IRStrobe::setPower(int lightIntensity)
{
    value_ = lightIntensity & 0b11111;
    sendParameter();
}

int IRStrobe::getPower() const
{
    return value_;
}

void IRStrobe::ignite()
{
    igniteStrobe_ = true;
}

// パラメータの種別
void IRStrobe::set(StrobeType type)
{
    type_ = type;
}

// パラメータの送信
// バッファにはLSBファーストで詰めていく
// 実際は11111(31)がフル発光、11(3)が1/128発光なのでそれを想定してパラメータ調整をする
void IRStrobe::sendParameter() const
{
    rmt_item32_t buffer[PatternBufferSize];
    uint32_t sendValue = (31 - value_) << ValueBitPosition;
    switch (type_)
    {
    case StrobeType::Manual:
        //             98765432109876543210
        sendValue |= 0b00000001100000111111;
        break;
    case StrobeType::ETTLPreStrobe:
        //             98765432109876543210
        sendValue |= 0b00000000100011000011;
        break;
    case StrobeType::ETTLStrobe:
        //             98765432109876543210
        sendValue |= 0b00000010100011110011;
        break;
    default:
        break;
    }

    for (int i = 0; i < PatternBufferSize; i++) {
        buffer[i].duration0 = HighDuration;
        buffer[i].duration1 = LowDuration;
        buffer[i].level1 = 0;
        buffer[i].level0 = sendValue & 0b1;
        sendValue = sendValue >> 1;
    }
    rmt_write_items(channel_, buffer, PatternBufferSize, true);
}

// 発光信号の送信
void IRStrobe::detectEvent()
{
    if (igniteStrobe_) {
        rmt_item32_t buffer = {
            HighDuration, 1, LowDuration, 0
        };
        rmt_write_items(channel_, &buffer, 1, true);
        igniteStrobe_ = false;
    }
}
