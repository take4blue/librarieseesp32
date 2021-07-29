#include <TaskControl.hpp>

class Test : public Take4::TaskControl<Test>
{
  private:
    volatile int counter_;
  public:
    Test() : counter_(0) {}
    ~Test() {}

    // カウンターをインクリメントしてそれを200ticks単位に表示する。
    void task()
    {
      for(;;++counter_) {
        printf("t = %d\n", counter_);
        vTaskDelay(200 / portTICK_PERIOD_MS);
      }
    }
};
Test test1;

extern "C"
void app_main()
{
  // test1のtaskを起動する
  test1.startProcess("test1", 4096, 5, 0);
}