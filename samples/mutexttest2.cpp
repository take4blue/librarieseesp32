#include <IOAccess.hpp>
#include <Spinlock.hpp>
#include <TaskControl.hpp>
#include <Timer.h>
#include <esp32/rom/ets_sys.h>
#include <mutex>

// mutexとSpinlockに関するテスト
// ets_delay_us(2000)部分にロックをかけて2つのタスクがどのように協調動作していくかを調べていく
// mutex、Spinlockと同一コア、異コアの組み合わせで見てみる

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

typedef std::mutex Lock; // これでmutex, Spinlockを切り替える

class Test2 : public Take4::TaskControl<Test2> {
  private:
    Lock &lock_;
    SquareWave pin_;

  public:
    Test2(Lock &lock)
        : lock_(lock)
        , pin_()
    {
    }
    ~Test2() {}

    void begin(gpio_num_t pin)
    {
        pin_.begin(pin);
    }

    // カウンターをインクリメントしてそれを200ticks単位に表示する。
    void task()
    {
        for (;;) {
            pin_.toggle();
            std::lock_guard<Lock> p(lock_);
            ets_delay_us(2000);
        }
    }
};

Lock lock1;
Test2 test1(lock1);
Lock lock2;
Test2 test2(lock1);

extern "C" void app_main()
{
    auto prio = uxTaskPriorityGet(nullptr);
    test1.begin(GPIO_NUM_2);
    test1.startProcess("test1", 4096, prio, 1); // ここで同一コア、異コアを切り替える
    test2.begin(GPIO_NUM_15);
    test2.startProcess("test2", 4096, prio, 0);
}