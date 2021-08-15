#include <IOAccess.hpp>
#include <Spinlock.hpp>
#include <TaskControl.hpp>
#include <esp32/rom/ets_sys.h>
#include <mutex>

class Test1 : public Take4::TaskControlV<Test1> {
  protected:
    volatile int counter_;
    gpio_num_t pin_;

  public:
    Test1()
        : counter_(0)
        , pin_(GPIO_NUM_0)
    {
    }
    virtual ~Test1() {}

    void begin(gpio_num_t pin)
    {
        pin_ = pin;
        gpio_pad_select_gpio(pin_);
        gpio_set_direction(pin_, GPIO_MODE_OUTPUT);
    }

    void pin()
    {
        // delay単位にピンのH/Lを繰り返す
        if ((counter_ % 2) == 0) {
            Take4::gpioSet(pin_);
        }
        else {
            Take4::gpioClear(pin_);
        }
        ets_delay_us(2000);
    }

    virtual void task()
    {
        for (;; ++counter_) {
            pin();
        }
    }
};

class Test2 : public Test1 {
  private:
    Take4::Spinlock lock_;

  public:
    Test2()
        : Test1()
        , lock_()
    {
    }
    virtual ~Test2() {}

    void begin(gpio_num_t pin)
    {
        Test1::begin(pin);
    }

    virtual void task()
    {
        // タスク全体をクリティカルセクションにしている
        std::lock_guard<Take4::Spinlock> a(lock_);
        for (;; ++counter_) {
            pin();
        }
    }
};

Test1 test1;
Test2 test2;

extern "C" void app_main()
{
    test1.begin(GPIO_NUM_15);
    test2.begin(GPIO_NUM_2);

    auto prio = uxTaskPriorityGet(nullptr);
    test1.startProcess("test1", 1028, prio, 0);
    test2.startProcess("test2", 1028, prio, 0);
}