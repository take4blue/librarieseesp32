#include <Task.h>

// 通常クラスのTaskとウォッチドッグタイマーのサンプル
class Test {
  private:
    Take4::Task task_;
    volatile int counter_;
    volatile bool exitFlag_;

    static void func(Take4::Task &task, void *data);

  public:
    Test()
        : task_()
        , counter_(0)
        , exitFlag_(false)
    {
    }
    ~Test() {}

    void start(UBaseType_t priority, BaseType_t core)
    {
        task_.startProcess("test1", 4096, priority, core, func, (void *)this);
    }

    // カウンターをインクリメントしてそれを200ticks単位に表示する。
    void task()
    {
        task_.addWatchDog();
        for (; !exitFlag_; ++counter_) {
            task_.resetWatchDog();
            printf("t = %d\n", counter_);
            if ((counter_ % 20) == 19) {
                vTaskDelay(2000 / portTICK_PERIOD_MS); // ここでWDTが動作するはず
            }
            else {
                vTaskDelay(200 / portTICK_PERIOD_MS);
            }
        }
        ets_printf("exit task\n");
        task_.deleteWatchDog();
    }

    void exitTask()
    {
        exitFlag_ = true;
    }
};

IRAM_ATTR void Test::func(Take4::Task &task, void *data)
{
    auto obj = (Test *)data;
    obj->task();
}

Test test1;

extern "C" void app_main()
{
    Take4::Task::initWatchDogTimer(1); // WDTを1秒で設定
    // test1のtaskを起動する
    auto prio = uxTaskPriorityGet(nullptr);
    test1.start(prio, 0);
}

// ウォッチドッグタイマーの割り込み処理
extern "C" void esp_task_wdt_isr_user_handler()
{
    ets_printf("lol\n");
    test1.exitTask();   // タスク終了フラグの設定
}