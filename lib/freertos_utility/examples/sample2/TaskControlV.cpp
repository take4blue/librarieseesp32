#include <TaskControl.hpp>

class Test1 : public Take4::TaskControlV<Test1>
{
  protected:
    volatile int counter_;
    Test1* child_;
    int no_;
  public:
    Test1(int no, Test1* child = nullptr) : counter_(0), child_(child), no_(no) {}
    virtual ~Test1() {}

    virtual void task()
    {
      for(;;++counter_) {
        printf("t%d = %d\n", no_, counter_);
        vTaskDelay(150 / portTICK_PERIOD_MS);
        if (child_) {
          child_->notify();
        }
        vTaskDelay(150 / portTICK_PERIOD_MS);
      }
    }
};

class Test2 : public Test1
{
  public:
  Test2(int no, Test1* child = nullptr) : Test1(no, child) {}
  virtual ~Test2() {}

  virtual void task()
  {
    for(;;++counter_) {
      uint32_t value;
      if (wait(value, pdMS_TO_TICKS(250)) == pdPASS) {
        printf("t%d = %d\n", no_, counter_);
        if (child_) {
          child_->notify();
        }
      }
    }
  }
};

Test2 test2(2);
Test2 test3(3, &test2);
Test1 test1(1, &test3);

extern "C"
void app_main()
{
  auto prio = uxTaskPriorityGet(nullptr);

  test2.startProcess("test2", 16384, prio, 1);
  test3.startProcess("test3", 16384, prio, 0);
  test1.startProcess("test1", 16384, prio, 0);
}