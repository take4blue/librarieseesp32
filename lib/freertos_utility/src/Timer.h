#pragma once
#include <driver/timer.h>
#include <functional>

namespace Take4 {

// タイマーのラッパークラス
// 割り込みを使わない場合ちょっとだけコストアップかもしれない
// 外部割込みはCallbackFunctionで定義しておきvoid*を自分自身に型変換して使用する
// そのためbeginでthisをdata(void*)に入れておくこと
// コールバック関数内でxQueueSendFromISRを使う場合pxHigherPriorityTaskWokenの返り値をreturnの判断材料にする
// それ以外はfalseリターンで良いようだ
class Timer {
  public:
    // コールバック呼び出し関数
    // 詳細はESP-IDFのtimer_isr_tのAPIを参照
    // コールバック内でxQueueSendFromISRを使う場合pxHigherPriorityTaskWokenの返り値をreturnの判断材料にする
    // それ以外はfalseリターンで良いようだ
    typedef std::function<bool(Timer &, void *)> CallbackFunction;

  private:
    volatile timer_group_t timerGroup_;
    volatile timer_idx_t timerEventIndex_;
    CallbackFunction function_;
    void *callbackData_;

    // 割り込み関数
    // 中身でT::taskをすぐに呼び出すようにしている
    static bool timerIntr(void *p);

  public:
    Timer();
    ~Timer();

    // タイマー初期化
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
    void begin(const timer_config_t &config, timer_group_t timerGroup = TIMER_GROUP_0, timer_idx_t timerEventIndex = TIMER_0);

    // タイマー割り込み初期化
    // config : 割り込みの初期化内容(以下は例)
    // timer_config_t config = {
    //     .alarm_en = TIMER_ALARM_EN,
    //     .counter_en = TIMER_PAUSE,
    //     .intr_type = TIMER_INTR_LEVEL,
    //     .counter_dir = TIMER_COUNT_DOWN,
    //     .auto_reload = TIMER_AUTORELOAD_DIS,
    //     .divider = 16,
    // }; // default clock source is APB
    // func : 割り込み関数(IRAM配置である必要がある)
    // data : 割り込み関数に渡すデータのポインタ(主に割り込み関数内でオブジェクトに戻すために利用)
    // timerGroup : タイマーグループ
    // timerEventIndex : タイマーインデックス
    void begin(const timer_config_t &config, CallbackFunction func, void *data, timer_group_t timerGroup = TIMER_GROUP_0, timer_idx_t timerEventIndex = TIMER_0);

    // タイマースタート
    void start();

    // タイマーポーズ
    void pause();

    // ハードウェアタイマーのカウントモードを設定
    // dir : タイマーのカウント方向
    void setCounterMode(timer_count_dir_t dir);

    // アラームイベント発生時のカウンタリロード機能の設定
    // reload : リロードのモード
    void setAutoReload(timer_autoreload_t reload);

    // タイマーのアラーム値の設定
    // value : アラーム値
    void setAlarmValue(uint64_t value);

    // アラーム値(μ秒単位)を計算
    // value : μ秒のアラーム値
    // dividevalue : timer_config_t.dividerの値
    // return : setAlarmValueのvalueに渡す値
    static uint64_t microTime(uint64_t value, uint32_t dividevalue);

    // タイマーのアラーム値の取り出し
    uint64_t getAlarmValue();

    // タイマーのカウント値の設定
    // value : カウント値
    void setCounterValue(uint64_t value);

    // タイマーカウント値の取り出し
    uint64_t getCounterValue();

    // タイマーアラームイベントの生成を有効または無効にします。
    // enable : タイマーアラーム機能を有効または無効にします。
    void setAlarm(timer_alarm_t enable);

    // 割り込みを有効にする
    void enable();

    // 割り込みを無効にする
    void disable();

    // 割り込みルーチン内での割り込み情報のクリア
    void clrIntrStatusISR();

    // 割り込みルーチン内での割り込み有効
    void enableAlarmISR();
};
} // namespace Take4
