#include "LEDStrobe.h"
#include <esp32/rom/ets_sys.h>

using namespace Take4;

// 以下は動作パラメータ
constexpr ledc_mode_t speedMode_ = LEDC_LOW_SPEED_MODE;
constexpr ledc_timer_t timerNo_ = LEDC_TIMER_0;
constexpr ledc_channel_t channel_ = LEDC_CHANNEL_0;
constexpr ledc_timer_bit_t dutyResolution_ = LEDC_TIMER_4_BIT;
constexpr uint32_t maxDuty_ = 1 << dutyResolution_;
constexpr uint32_t lowDuty_ = 2;
constexpr uint32_t stepDuty_ = maxDuty_ - lowDuty_;
constexpr uint16_t delayTime_ = 14;
constexpr size_t maxPower = 22;

const uint16_t flashTime[] = {
    2400,   // 1/1  31
    1835,
    1270,
    706,    // 1/2  28
    613,
    520,
    427,    // 1/4  25
    334,
    241,
    144,    // 1/8  22
    130,
    116,
    102,    // 1/16 19
    88,
    74,
    55,     // 1/32 16
    50,
    45,
    40,     // 1/64 13
    35,
    30,
    23,     // 1/128 10
};

LEDStrobe::LEDStrobe()
: led_(GPIO_NUM_0)
, power_(0)
, igniteStrobe_(false)
, flashTime_(flashTime)
{
}

LEDStrobe::~LEDStrobe()
{
}

void LEDStrobe::begin(gpio_num_t led)
{
    led_ = led;
    gpio_config_t gpio = {
        .pin_bit_mask = 1ULL << led,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&gpio);
}

void LEDStrobe::setPower(int power)
{
    power_ = power;
    if (power_ >= maxPower) {
        power_ = maxPower - 1;
    }
}

int LEDStrobe::getPower() const
{
    return power_;
}

void LEDStrobe::ignite()
{
    igniteStrobe_ = true;
}

void LEDStrobe::detectEvent()
{
    if (igniteStrobe_) {
        gpio_set_level(led_, 1);
        ets_delay_us(flashTime_[power_]);
        gpio_set_level(led_, 0);
        igniteStrobe_ = false;
    }
}

void LEDStrobe::setFlashTime(const uint16_t* values)
{
    if (values == nullptr) {
        flashTime_ = flashTime;
    }
    else {
        flashTime_ = values;
    }
}