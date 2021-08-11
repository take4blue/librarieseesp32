#include <IOAccess.hpp>
#include <Timer.hpp>

// 3ms単位にGPIOのH/Lを繰り返すだけのクラス
class Test1 : public Take4::Timer<Test1> {
  protected:
    volatile int counter_;
    gpio_num_t pin_;

  public:
    Test1()
        : counter_(0)
        , pin_(GPIO_NUM_0)
    {
    }
    ~Test1() {}

    void begin(gpio_num_t pin)
    {
        // ピンの初期化
        pin_ = pin;
        gpio_pad_select_gpio(pin_);
        gpio_set_direction(pin_, GPIO_MODE_OUTPUT);

        timer_config_t config = {
            .alarm_en = TIMER_ALARM_EN,
            .counter_en = TIMER_PAUSE,
            .intr_type = TIMER_INTR_LEVEL,
            .counter_dir = TIMER_COUNT_UP,
            .auto_reload = TIMER_AUTORELOAD_EN,
            .divider = 80,
        };

        // タイマーの初期化
        Take4::Timer<Test1>::begin(config);
        auto val = microTime(3000, config.divider);
        setAlarmValue(val);
        setCounterValue(0ULL);
        start();
    }

    void interrupt()
    {
        ++counter_;
        // delay単位にピンのH/Lを繰り返す
        if ((counter_ % 2) == 0) {
            Take4::gpioSet(pin_);
        }
        else {
            Take4::gpioClear(pin_);
        }
    }
};

Test1 test1;

extern "C" void app_main()
{
    test1.begin(GPIO_NUM_15);
}