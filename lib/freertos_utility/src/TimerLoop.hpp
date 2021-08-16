#pragma once
#include <esp_timer.h>

namespace Take4 {
// ESP_TIMERのラッパークラス
// 派生側でvoid timerAction()をpublicに記述する。
// once()やperiodic()で指定した時間経過したらtimerAction()が呼びだされる
template <typename T>
class TimerLoop {
  private:
    esp_timer_handle_t handle_;

    // 内部的な一次受けコールバック関数
    static void intr(void *data)
    {
        auto user = (T *)data;
        user->timerAction();
    }

    // 2次受けコールバックで派生先ではpublicで記述する。
    void timerAction();

  public:
    TimerLoop()
        : handle_(nullptr)
    {
    }
    ~TimerLoop()
    {
        end();
    }

    // タイマーループの開始
    // light_sleep_enableがtrueの場合skip_unhandled_eventsをtrueにするとタイマーループによるウェイクアップはされないらしい
    // name : タスク名
    // skip_unhandled_events : trueの場合タイマーイベントがたまっていても1回しか実施しない
    void begin(const char *name, bool skip_unhandled_events)
    {
        if (handle_ == nullptr) {
            esp_timer_create_args_t config = {
                .callback = &intr,
                .arg = (void *)this,
                .dispatch_method = ESP_TIMER_TASK,
                .name = name,
                .skip_unhandled_events = skip_unhandled_events,
            };
            ESP_ERROR_CHECK(esp_timer_create(&config, &handle_));
        }
    }

    // タイマーループの終了
    void end()
    {
        esp_timer_delete(handle_);
        handle_ = nullptr;
    }

    // 1度だけ実施のタイマー設定
    // timeout : タイマー期間(μ秒)
    void once(uint64_t timeout)
    {
        ESP_ERROR_CHECK(esp_timer_start_once(handle_, timeout));
    }

    // 何度でも実施のタイマー設定
    // timeout : タイマー期間(μ秒)
    void periodic(uint64_t timeout)
    {
        ESP_ERROR_CHECK(esp_timer_start_periodic(handle_, timeout));
    }

    // タイマーの停止
    void stop()
    {
        ESP_ERROR_CHECK(esp_timer_stop(handle_));
    }

    // 基礎となるタイマーが開始されてからのマイクロ秒数
    static int64_t getTime()
    {
        return esp_timer_get_time();
    }
};
} // namespace Take4
