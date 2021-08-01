#include <IRStrobe.h>
#include <LEDStrobe.h>

#include <Readline.h>
#include <TaskControl.hpp>
#include <string.h>

const gpio_num_t irPin = GPIO_NUM_4;
const gpio_num_t ledPin = GPIO_NUM_32;

Take4::IRStrobe ir;
Take4::LEDStrobe led;

class Controler : public Take4::TaskControl<Controler>
{
  private:
    Take4::IStrobe *strobe_;

  public:
    Controler() : strobe_(nullptr) {}
    ~Controler() {}

    void task()
    {
      for (;;) {
        if (strobe_) {
          strobe_->detectEvent();
        }
      }
    }

    void set(Take4::IStrobe* strobe)
    {
      strobe_ = strobe;
    }

    void flash(int val)
    {
      if (strobe_) {
        strobe_->setPower(val);
        strobe_->ignite();
      }
    }
};

Controler ctrl;
Take4::Readline reader;

extern "C"
void app_main()
{
  ir.begin(irPin, RMT_CHANNEL_0);
  led.begin(ledPin);
  ctrl.set(&led);
  auto prio = uxTaskPriorityGet(nullptr);
  ctrl.startProcess("Control", 4096, prio, 1);

  for (;;) {
    if (reader.read("> ")) {
      auto line = reader.get();
      switch(line[0]) {
        case 'i':
          ctrl.set(&ir);
          break;
        case 'l':
          ctrl.set(&led);
          break;
        default:
          char* errPos;
          auto val = strtol(line, &errPos, 10);
          ctrl.flash(val);
          break;
      }
    }
  } 
}