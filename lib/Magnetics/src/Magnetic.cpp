#include "Magnetic.h"
#if defined(_GNU_SOURCE)
// max, minの呼び方をarduino側と合わせるため
#include <algorithm>
using namespace std;
#endif
using namespace Take4;

Magnetic::Magnetic()
    : origin_(0)
    , sum_(0)
    , prevValue_(0)
    , current_(0)
    , max_(0)
    , min_(0)
{
}

void Magnetic::begin()
{
#ifdef ARDUINO_ESP32_DEV
    adc1_config_channel_atten(channel_, ADC_ATTEN_DB_11);
#endif
    // 磁石が近傍にない場合の基準値を決める。
    int origin = 0;
    for (uint8_t i = 0; i < maxOriginRetry_; i++) {
        origin += sensorValue();
    }
    origin_ = max_ = min_ = prevValue_ = current_ = (origin / maxOriginRetry_);
    for (uint8_t i = 0; i < maxGetRetry_; i++) {
        value_[i] = origin_;
    }
    sum_ = origin_ * maxGetRetry_;

    // デバイス単位での揺れ幅を計測し、それを閾値の上乗せ分とする
    uint16_t wMax = max_;
    uint16_t wMin = min_;
    for (uint8_t i = 0; i < maxOriginRetry_; i++) {
        auto work = (uint16_t)sensorValue();
        wMax = max(wMax, work);
        wMin = min(wMin, work);
    }
    indexThreshold_ = max(wMax - origin_, origin_ - wMin);
}

int Magnetic::sensorValue() const
{
#ifdef ARDUINO_ESP32_DEV
    return adc1_get_raw(channel_);
#else
    return analogRead(channel_);
#endif
}

void Magnetic::calibration()
{
    auto value = sensorValue();
    max_ = max(max_, (uint16_t)value);
    min_ = min(min_, (uint16_t)value);
}

int Magnetic::get() const
{
    return (int)current_ - (int)origin_;
}

bool Magnetic::check(uint8_t position, uint8_t threshold)
{
    sum_ -= value_[position];
    value_[position] = sensorValue();
    sum_ += value_[position];
    current_ = sum_ / maxGetRetry_;
    if (abs((int)prevValue_ - (int)current_) > (threshold + indexThreshold_)) {
        prevValue_ = current_;
        return true;
    } else {
        return false;
    }
}

void Magnetic::set(Port_t val)
{
    channel_ = val;
}

uint8_t Magnetic::retryNum()
{
    return maxGetRetry_;
}
