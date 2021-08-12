#pragma once
#include <driver/timer.h>
#include <esp_log.h>
#include <functional>
#include <hal/timer_types.h>

namespace Take4 {
// タイマー割り込みのクラス側の実装例
#define TimerIntrImpl(className)                 \
    IRAM_ATTR bool className::timerIntr(void *p) \
    {                                            \
        className *user = (className *)p;        \
        user->interrupt();                       \
        user->clrIntrStatusISR();                \
        user->enableAlarmISR();                  \
        return false;                            \
    }
// TimerIntrImplに合わせたクラス内での定義例
#define TimerIntrDefine() \
    static bool timerIntr(void *p)

// タイマー割り込みのラッパークラス
// T : 組み込みクラス
// void interrupt()が呼び出される割り込みルーチン
template <class T>
class TimerT {
  private:
    volatile timer_group_t timerGroup_;
    volatile timer_idx_t timerEventIndex_;

    // 割り込み関数
    // 中身でT::taskをすぐに呼び出すようにしている
    static bool timerIntr(void *p)
    {
        T *user = (T *)p;
        user->T::interrupt();
        user->clrIntrStatusISR();
        user->enableAlarmISR();
        return false;
    }

    // ダミー
    void interrupt();

  public:
    // コールバック呼び出し関数
    // 詳細はESP-IDFのtimer_isr_tのAPIを参照
    // コールバック内でxQueueSendFromISRを使う場合pxHigherPriorityTaskWokenの返り値をreturnの判断材料にする
    // それ以外はfalseリターンで良いようだ
    typedef bool (*InterruptFunction)(void *);

    TimerT()
        : timerGroup_(TIMER_GROUP_0)
        , timerEventIndex_(TIMER_0)
    {
    }
    ~TimerT() {}

    // 割り込み設定(直接呼出しの割り込みは非IRAM形式)
    // config : 割り込みの初期化内容(以下は例)
    // timer_config_t config = {
    //     .alarm_en = TIMER_ALARM_EN,
    //     .counter_en = TIMER_PAUSE,
    //     .intr_type = TIMER_INTR_LEVEL,
    //     .counter_dir = TIMER_COUNT_DOWN,
    //     .auto_reload = TIMER_AUTORELOAD_DIS,
    //     .divider = 16,
    // }; // default clock source is APB
    // timerGroup : タイマーグループ
    // timerEventIndex : タイマーインデックス
    void begin(const timer_config_t &config, timer_group_t timerGroup = TIMER_GROUP_0, timer_idx_t timerEventIndex = TIMER_0)
    {
        timerGroup_ = timerGroup;
        timerEventIndex_ = timerEventIndex;

        // タイマー割り込みの初期化
        ESP_ERROR_CHECK(timer_init(timerGroup_, timerEventIndex_, &config));
        ESP_ERROR_CHECK(timer_isr_callback_add(timerGroup_, timerEventIndex_,
                                               TimerT::timerIntr, (void *)this, 0));
        // ESP_LOGV(TAG_, "begin\n");
    }

    // 割り込み設定(IRAM形式)
    // 呼び出し関数はクラス定義側で用意する:TimerIntrImplマクロが例
    // config : 割り込みの初期化内容(以下は例)
    // timer_config_t config = {
    //     .alarm_en = TIMER_ALARM_EN,
    //     .counter_en = TIMER_PAUSE,
    //     .intr_type = TIMER_INTR_LEVEL,
    //     .counter_dir = TIMER_COUNT_DOWN,
    //     .auto_reload = TIMER_AUTORELOAD_DIS,
    //     .divider = 16,
    // }; // default clock source is APB
    // func : コールバックで呼び出す関数でIRAMに配置されている必要がある。TimerIntrImplを使う場合はクラス名::timerIntrにする
    // timerGroup : タイマーグループ
    // timerEventIndex : タイマーインデックス
    void begin(const timer_config_t &config, InterruptFunction func, timer_group_t timerGroup = TIMER_GROUP_0, timer_idx_t timerEventIndex = TIMER_0)
    {
        timerGroup_ = timerGroup;
        timerEventIndex_ = timerEventIndex;

        // タイマー割り込みの初期化
        ESP_ERROR_CHECK(timer_init(timerGroup_, timerEventIndex_, &config));
        ESP_ERROR_CHECK(timer_isr_callback_add(timerGroup_, timerEventIndex_,
                                               func, (void *)this, ESP_INTR_FLAG_IRAM));
        // ESP_LOGV(TAG_, "begin\n");
    }

    // タイマースタート
    void start()
    {
        // ESP_LOGV(TAG_, "start\n");
        ESP_ERROR_CHECK(timer_start(timerGroup_, timerEventIndex_));
    }

    // タイマーポーズ
    void pause()
    {
        // ESP_LOGV(TAG_, "pause\n");
        ESP_ERROR_CHECK(timer_pause(timerGroup_, timerEventIndex_));
    }

    // ハードウェアタイマーのカウントモードを設定
    // dir : タイマーのカウント方向
    void setCounterMode(timer_count_dir_t dir)
    {
        // ESP_LOGV(TAG_, "setCounterMode\n");
        ESP_ERROR_CHECK(timer_set_counter_mode(timerGroup_, timerEventIndex_, dir));
    }

    // アラームイベント発生時のカウンタリロード機能の設定
    // reload : リロードのモード
    void setAutoReload(timer_autoreload_t reload)
    {
        // ESP_LOGV(TAG_, "setAutoReload\n");
        ESP_ERROR_CHECK(timer_set_auto_reload(timerGroup_, timerEventIndex_, reload));
    }

    // タイマーのアラーム値の設定
    // value : アラーム値
    void setAlarmValue(uint64_t value)
    {
        // ESP_LOGV(TAG_, "setAlarmValue %lld\n", value);
        ESP_ERROR_CHECK(timer_set_alarm_value(timerGroup_, timerEventIndex_, value));
    }

    // アラーム値(μ秒単位)を計算
    // value : μ秒のアラーム値
    // dividevalue : timer_config_t.dividerの値
    // return : setAlarmValueのvalueに渡す値
    static uint64_t microTime(uint64_t value, uint32_t dividevalue)
    {
        return value * (TIMER_BASE_CLK / dividevalue / 1000000);
    }

    // タイマーのアラーム値の取り出し
    uint64_t getAlarmValue()
    {
        uint64_t value;
        if (timer_get_alarm_value(timerGroup_, timerEventIndex_, &value) == ESP_OK) {
            return value;
        }
        else {
            return 0;
        }
    }

    // タイマーのカウント値の設定
    // value : カウント値
    void setCounterValue(uint64_t value)
    {
        // ESP_LOGV(TAG_, "setCounterValue %lld\n", value);
        ESP_ERROR_CHECK(timer_set_counter_value(timerGroup_, timerEventIndex_, value));
    }

    // タイマーカウント値の取り出し
    uint64_t getCounterValue()
    {
        uint64_t value;
        if (timer_get_counter_value(timerGroup_, timerEventIndex_, &value) == ESP_OK) {
            return value;
        }
        else {
            return 0;
        }
    }

    // タイマーアラームイベントの生成を有効または無効にします。
    // enable : タイマーアラーム機能を有効または無効にします。
    void setAlarm(timer_alarm_t enable)
    {
        // ESP_LOGV(TAG_, "setAlarm\n");
        ESP_ERROR_CHECK(timer_set_alarm(timerGroup_, timerEventIndex_, enable));
    }

    // 割り込みを有効にする
    void enable()
    {
        // ESP_LOGV(TAG_, "enable\n");
        ESP_ERROR_CHECK(timer_enable_intr(timerGroup_, timerEventIndex_));
    }

    // 割り込みを無効にする
    void disable()
    {
        // ESP_LOGV(TAG_, "disable\n");
        ESP_ERROR_CHECK(timer_disable_intr(timerGroup_, timerEventIndex_));
    }

    // 割り込みルーチン内での割り込み情報のクリア
    void clrIntrStatusISR()
    {
        timer_group_clr_intr_status_in_isr(timerGroup_, timerEventIndex_);
    }

    // 割り込みルーチン内での割り込み有効
    void enableAlarmISR()
    {
        timer_group_enable_alarm_in_isr(timerGroup_, timerEventIndex_);
    }
};
} // namespace Take4
