#include <TaskControl.hpp>

// ウォッチドッグタイマーをタスクに組み込んだサンプル
class Test : public Take4::TaskControl<Test> {
  private:
    volatile int counter_;
    volatile bool exitFlag_;

  public:
    Test()
        : counter_(0)
        , exitFlag_(false)
    {
    }
    ~Test() {}

    // カウンターをインクリメントしてそれを200ticks単位に表示する。
    void task()
    {
        addWatchDog();
        for (; !exitFlag_; ++counter_) {
            resetWatchDog();
            printf("t = %d\n", counter_);
            if ((counter_ % 20) == 19) {
                vTaskDelay(2000 / portTICK_PERIOD_MS); // ここでWDTが動作するはず
            }
            else {
                vTaskDelay(200 / portTICK_PERIOD_MS);
            }
        }
        ets_printf("exit task\n");
        deleteWatchDog();
    }

    void exitTask()
    {
        exitFlag_ = true;
    }
};
Test test1;

extern "C" void app_main()
{
    test1.initWatchDogTimer(1);
    auto prio = uxTaskPriorityGet(nullptr);
    // test1のtaskを起動する
    test1.startProcess("test1", 4096, prio, 0);
}

// ウォッチドッグタイマーの割り込み処理
extern "C" void esp_task_wdt_isr_user_handler()
{
    ets_printf("lol\n");
    test1.exitTask();   // タスク終了フラグの設定
}