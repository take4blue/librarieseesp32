#include <IOAccess.hpp>
#include <TimerLoop.hpp>
#include <esp32/rom/ets_sys.h>

// ESP_TIMERのラッパークラスのサンプル
// ループ内でカウンターに合わせてディレイを入れ、指定時間内にタイマー処理が終わらない条件を想定
// skip_unhandled_eventsをfalseにしているのでperiodicの500μ秒に納まらない場合処理がキューに貯められた感じになっている
// そのため一部方形波の間隔が500μ秒間隔より短い部分が出る
// trueの場合はキューにはたまらないので500μ秒未満になることはないはずだが、条件によっては1回だけ短いのがある場合がある
class Test : public Take4::TimerLoop<Test> {
  private:
    gpio_num_t pin_;
    uint8_t counter_;

  public:
    Test()
        : Take4::TimerLoop<Test>()
        , pin_()
        , counter_(0)
    {
    }
    ~Test() {}

    void timerAction()
    {
        ++counter_;
        // 方形波を出す処理
        if ((counter_ % 2) == 0) {
            Take4::gpioSet(pin_);
        }
        else {
            Take4::gpioClear(pin_);
        }
        ets_delay_us(counter_ * 3);     // skip_unhandled_events判断用にディレイを入れている
    }

    void begin(gpio_num_t pin)
    {
        pin_ = pin;
        gpio_pad_select_gpio(pin_);
        gpio_set_direction(pin_, GPIO_MODE_OUTPUT);
        Take4::TimerLoop<Test>::begin("Test", true);
    }
};

Test test1;

extern "C" void app_main(void)
{
    test1.begin(GPIO_NUM_15);
    test1.periodic(500);
}