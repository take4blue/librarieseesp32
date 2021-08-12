#include <IOAccess.hpp>
#include <Timer.h>

// 3ms単位にGPIOのH/Lを繰り返すだけのクラス
class Test1 {
  private:
    volatile int counter_;
    gpio_num_t pin_;
    Take4::Timer timer_;

    static bool intr(Take4::Timer &timer, void *p);

  public:
    Test1()
        : counter_(0)
        , pin_(GPIO_NUM_0)
        , timer_()
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

        // タイマーの初期化(IRAM形式)
        timer_.begin(config, Test1::intr, this);
        auto val = timer_.microTime(3000, config.divider);
        timer_.setAlarmValue(val);
        timer_.setCounterValue(0ULL);
        timer_.start();
    }

    bool IRAM_ATTR interrupt()
    {
        ++counter_;
        // delay単位にピンのH/Lを繰り返す
        if ((counter_ % 2) == 0) {
            Take4::gpioSet(pin_);
        }
        else {
            Take4::gpioClear(pin_);
        }

        return false;
    }
};

// IRAM配置なstatic関数はクラス内で定義できないようなのでここに実装している
IRAM_ATTR bool Test1::intr(Take4::Timer &timer, void *p)
{
    auto user = (Test1 *)p;
    return user->interrupt();
}

Test1 test1;

extern "C" void app_main()
{
    test1.begin(GPIO_NUM_15);
}