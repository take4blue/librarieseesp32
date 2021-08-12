#include "Timer.h"

namespace Take4 {
IRAM_ATTR bool Timer::timerIntr(void *p)
{
    auto user = (Timer *)p;
    user->function_(*user, user->callbackData_);
    user->clrIntrStatusISR();
    user->enableAlarmISR();
    return false;
}

Timer::Timer()
    : timerGroup_(TIMER_GROUP_0)
    , timerEventIndex_(TIMER_0)
    , function_()
    , callbackData_(nullptr)
{
}

Timer::~Timer() {}

void Timer::begin(const timer_config_t &config, timer_group_t timerGroup, timer_idx_t timerEventIndex)
{
    timerGroup_ = timerGroup;
    timerEventIndex_ = timerEventIndex;

    // タイマー割り込みの初期化
    ESP_ERROR_CHECK(timer_init(timerGroup_, timerEventIndex_, &config));
    // ESP_LOGV(TAG_, "begin\n");
}

void Timer::begin(const timer_config_t &config, CallbackFunction func, void *data, timer_group_t timerGroup, timer_idx_t timerEventIndex)
{
    timerGroup_ = timerGroup;
    timerEventIndex_ = timerEventIndex;
    function_ = func;
    callbackData_ = data;

    // タイマー割り込みの初期化
    ESP_ERROR_CHECK(timer_init(timerGroup_, timerEventIndex_, &config));
    ESP_ERROR_CHECK(timer_isr_callback_add(timerGroup_, timerEventIndex_,
                                           Timer::timerIntr, (void *)this, ESP_INTR_FLAG_IRAM));
    // ESP_LOGV(TAG_, "begin\n");
}

void Timer::start()
{
    // ESP_LOGV(TAG_, "start\n");
    ESP_ERROR_CHECK(timer_start(timerGroup_, timerEventIndex_));
}

void Timer::pause()
{
    // ESP_LOGV(TAG_, "pause\n");
    ESP_ERROR_CHECK(timer_pause(timerGroup_, timerEventIndex_));
}

void Timer::setCounterMode(timer_count_dir_t dir)
{
    // ESP_LOGV(TAG_, "setCounterMode\n");
    ESP_ERROR_CHECK(timer_set_counter_mode(timerGroup_, timerEventIndex_, dir));
}

void Timer::setAutoReload(timer_autoreload_t reload)
{
    // ESP_LOGV(TAG_, "setAutoReload\n");
    ESP_ERROR_CHECK(timer_set_auto_reload(timerGroup_, timerEventIndex_, reload));
}

void Timer::setAlarmValue(uint64_t value)
{
    // ESP_LOGV(TAG_, "setAlarmValue %lld\n", value);
    ESP_ERROR_CHECK(timer_set_alarm_value(timerGroup_, timerEventIndex_, value));
}

uint64_t Timer::microTime(uint64_t value, uint32_t dividevalue)
{
    return value * (TIMER_BASE_CLK / dividevalue / 1000000);
}

uint64_t Timer::getAlarmValue()
{
    uint64_t value;
    if (timer_get_alarm_value(timerGroup_, timerEventIndex_, &value) == ESP_OK) {
        return value;
    }
    else {
        return 0;
    }
}

void Timer::setCounterValue(uint64_t value)
{
    // ESP_LOGV(TAG_, "setCounterValue %lld\n", value);
    ESP_ERROR_CHECK(timer_set_counter_value(timerGroup_, timerEventIndex_, value));
}

uint64_t Timer::getCounterValue()
{
    uint64_t value;
    if (timer_get_counter_value(timerGroup_, timerEventIndex_, &value) == ESP_OK) {
        return value;
    }
    else {
        return 0;
    }
}

void Timer::setAlarm(timer_alarm_t enable)
{
    // ESP_LOGV(TAG_, "setAlarm\n");
    ESP_ERROR_CHECK(timer_set_alarm(timerGroup_, timerEventIndex_, enable));
}

void Timer::enable()
{
    // ESP_LOGV(TAG_, "enable\n");
    ESP_ERROR_CHECK(timer_enable_intr(timerGroup_, timerEventIndex_));
}

void Timer::disable()
{
    // ESP_LOGV(TAG_, "disable\n");
    ESP_ERROR_CHECK(timer_disable_intr(timerGroup_, timerEventIndex_));
}

void Timer::clrIntrStatusISR()
{
    timer_group_clr_intr_status_in_isr(timerGroup_, timerEventIndex_);
}

void Timer::enableAlarmISR()
{
    timer_group_enable_alarm_in_isr(timerGroup_, timerEventIndex_);
}
} // namespace Take4
