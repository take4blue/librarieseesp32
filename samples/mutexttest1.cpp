#include <IOAccess.hpp>
#include <Spinlock.hpp>
#include <TaskControl.hpp>
#include <Timer.h>
#include <esp32/rom/ets_sys.h>
#include <mutex>

// Spinlockとmutexで排他制御をする場合、割り込みの対応がどのように変わるのかを
// テストするためのサンプルプログラム
// Test2クラスのLockをstd::mutexとTake4::Spinlockに変更して、その変化をロジックアナライザで見る

class SquareWave {
  private:
    volatile uint8_t counter_;
    gpio_num_t pin_;

  public:
    SquareWave()
        : counter_(0)
        , pin_(GPIO_NUM_0)
    {
    }
    ~SquareWave() {}

    void begin(gpio_num_t pin)
    {
        pin_ = pin;
        gpio_pad_select_gpio(pin_);
        gpio_set_direction(pin_, GPIO_MODE_OUTPUT);
    }

    void toggle()
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

class Test2 : public Take4::TaskControl<Test2> {
  private:
    typedef std::mutex Lock;
    Lock lock_;
    Take4::Timer timer_;
    SquareWave pin1_;
    SquareWave pin2_;

    static bool intr1(Take4::Timer &timer, void *p);

  public:
    Test2()
        : lock_()
        , timer_()
        , pin1_()
        , pin2_()
    {
    }
    ~Test2() {}

    void begin(gpio_num_t pin1, gpio_num_t pin2)
    {
        pin1_.begin(pin1);
        pin2_.begin(pin2);
        timer_config_t config = {
            .alarm_en = TIMER_ALARM_EN,
            .counter_en = TIMER_PAUSE,
            .intr_type = TIMER_INTR_LEVEL,
            .counter_dir = TIMER_COUNT_UP,
            .auto_reload = TIMER_AUTORELOAD_EN,
            .divider = 80,
        };

        // タイマーの初期化(IRAM形式)
        timer_.begin(config, Test2::intr1, this);
        auto val = timer_.microTime(3000, config.divider);
        timer_.setAlarmValue(val);
        timer_.setCounterValue(0ULL);
    }

    bool intr()
    {
        pin2_.toggle();
        return false;
    }

    // カウンターをインクリメントしてそれを200ticks単位に表示する。
    void task()
    {
        timer_.start();
        for (;;) {
            pin1_.toggle();
            std::lock_guard<Lock> p(lock_);
            ets_delay_us(500000);
        }
    }
};

IRAM_ATTR bool Test2::intr1(Take4::Timer &timer, void *p)
{
    auto user = (Test2 *)p;
    return user->intr();
}

Test2 test2;

extern "C" void app_main()
{
    auto prio = uxTaskPriorityGet(nullptr);
    test2.begin(GPIO_NUM_15, GPIO_NUM_2);
    test2.startProcess("test1", 4096, prio, 0);
}