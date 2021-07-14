#include "Magnetics.h"

#if defined(IDF_VER)
#include <esp_timer.h>
namespace {
    uint32_t millis()
    {
        return esp_timer_get_time() / 1000;
    }
}
#endif

using namespace Take4;

Magnetics::Magnetics(uint8_t threshold, uint32_t interval)
    : nData_(0)
    , threshold_(threshold)
    , position_(0)
    , timer_(0)
    , interval_(interval)
{
}

bool Magnetics::add(Port_t channel)
{
    if (nData_ < maxData_) {
        data_[nData_].set(channel);
        nData_++;
        return true;
    } else {
        return false;
    }
}

void Magnetics::begin()
{
#ifdef ESP32
    adc1_config_width(ADC_WIDTH_BIT_12);
#endif
    // 現状の基準値を求める
    for (uint8_t j = 0; j < nData_; j++) {
        data_[j].begin();
    }
    timer_ = millis();
}

bool Magnetics::check()
{
    uint32_t base = ((millis() - timer_) / interval_) + 1;
    bool updated = false;
    for (uint8_t i = 0; i < nData_; i++) {
        if (data_[i].check(position_, threshold_ / base)) {
            updated = true;
        }
    }
    position_ = (position_ + 1) % Magnetic::retryNum();
    if (updated) {
        timer_ = millis();
    }
    return updated;
}

void Magnetics::calibration(uint8_t time)
{
    for (auto startTime = millis(); (millis() - startTime) < (time * 1000);) {
        for (uint8_t i = 0; i < nData_; i++) {
            data_[i].calibration();
        }
    }
}

int Magnetics::get(uint8_t no) const
{
    if (no < nData_) {
        return data_[no].get();
    } else {
        return 0;
    }
}

uint8_t Magnetics::nPort() const
{
    return nData_;
}