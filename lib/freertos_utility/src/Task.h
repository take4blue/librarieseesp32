#pragma once
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <functional>

namespace Take4 {
// FreeRTOSのTask関連のラッパークラス
// データ容量を少なくしたい場合はテンプレートクラス側のを利用するといい
// Taskを使う場合、利用側クラスでstartProcessを隠蔽した起動メソッドを用意するのがいい
class Task {
  public:
    // コールバック呼び出し関数
    // xTaskCreatePinnedToCoreで呼び出される関数に第1引数がTaskになったもの
    typedef std::function<void(Task &, void *)> CallbackFunction;

  private:
    TaskHandle_t taskHandle_;
    CallbackFunction function_;
    void *callbackData_;

    // 呼び出されたタスク
    static void task(void *p);

  public:
    Task();
    ~Task();

    // プロセスの開始
    // 多重起動は防止される
    // taskName : タスク名
    // stackSize : タスクに割り当てるスタックサイズ
    // priority : 優先順位
    // core : 起動コア番号
    // func : 割り込み関数
    // data : 割り込み関数に渡すデータのポインタ(呼び出し側を(void*)thisで渡すのが望ましい)
    void startProcess(const char *taskName, uint32_t stackSize, UBaseType_t priority, BaseType_t core, CallbackFunction func, void *data);

    // タスクの一時停止
    void suspend();

    // タスクの一時停止の再開
    void resume();

    // タスクの状態取得
    // return : https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos.html#_CPPv410eTaskState
    eTaskState status() const;

    // 通知の送信
    // 参照 : https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos.html#_CPPv418xTaskGenericNotify12TaskHandle_t8uint32_t13eNotifyActionP8uint32_t
    // 前パラメータ省略するとxTaskNotifyGiveと同等になる
    // pulPreviousNotificationValueを省略するとxTaskNotifyと同等になる
    BaseType_t notify(uint32_t ulValue = 0, eNotifyAction eAction = eIncrement, uint32_t *pulPreviousNotificationValue = nullptr);

    // 割り込みからの通知の送信
    // 参照 : https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos.html#_CPPv425xTaskGenericNotifyFromISR12TaskHandle_t8uint32_t13eNotifyActionP8uint32_tP10BaseType_t
    // pulPreviousNotificationValue : 省略可能
    BaseType_t notifyFromISR(uint32_t ulValue, eNotifyAction eAction, BaseType_t *pxHigherPriorityTaskWoken, uint32_t *pulPreviousNotificationValue = nullptr);

    // 通知の受け待ち
    // 参照 : https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos.html#_CPPv415xTaskNotifyWait8uint32_t8uint32_tP8uint32_t10TickType_t
    // pulNotificationValue : 参照OUT
    // ulBitsToClearOnEntry, ulBitsToClearOnExitを省略した場合entry/exit時のビットクリアはしない
    BaseType_t wait(uint32_t &pulNotificationValue, TickType_t xTicksToWait, uint32_t ulBitsToClearOnEntry = 0, uint32_t ulBitsToClearOnExit = 0);

    // 全タスクの一時停止
    static void suspendAll();

    // 全タスクの一時停止解除
    static void resumeAll();

    // タスクウォッチドッグタイマーの初期化
    // app_main内で呼び出すのが良いみたいだ
    // timeout : タイムアウト時間(秒)
    // panic : パニックハンドラーを実行するかどうか(デフォルトfalse)
    // panicがtrueだとabort()後リブートがかかるようだ
    static void initWatchDogTimer(uint32_t timeout, bool panic = false);

    // タスクウォッチドッグを現在のタスクに割り当てる
    void addWatchDog();

    // タスクウォッチドッグを現在のタスクから解除する
    void deleteWatchDog();

    // タスクウォッチドッグタイマーをリセットする
    void resetWatchDog();
};
} // namespace Take4
