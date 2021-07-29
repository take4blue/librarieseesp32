#pragma once
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace Take4
{
    // FreeRTOSのTask関連のラッパークラス
    // Tクラスにはpublicでvoid task()というメソッドを用意しておくこと
    // 用意していないとコンパイルは通るがリンクでエラーになる
    // 仮想関数を使う場合はTaskControlVを使う
    // 参照 : https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos.html#task-api
    template <class T>
    class TaskControl {
    private:
        TaskHandle_t taskHandle_;

        // 呼び出されたタスク
        // 中身でT::taskをすぐに呼び出すようにしている
        static IRAM_ATTR void task(void *p)
        {
            T* user = (T*)p;
            user->T::task();
            // タスクが終了したので削除処理を実施する。
            TaskControl* task = (TaskControl*)p;
            vTaskDelete(nullptr);
            task->taskHandle_ = nullptr;
        }

        // ダミー
        void task();

    public:
        TaskControl() : taskHandle_(nullptr) {}
        ~TaskControl() {}

        // プロセスの開始
        // taskName : タスク名
        // stackSize : タスクに割り当てるスタックサイズ
        // priority : 優先順位
        // core : 起動コア番号
        void startProcess(const char* taskName, uint32_t stackSize, UBaseType_t priority, BaseType_t core)
        {
            xTaskCreatePinnedToCore(TaskControl::task, taskName,
                stackSize, (void*)this, priority, &taskHandle_, core);
        }

        // タスクの一時停止
        void suspend()
        {
            vTaskSuspend(taskHandle_);
        }

        // タスクの一時停止の再開
        void resume()
        {
            vTaskResume(taskHandle_);
        }

        // タスクの状態取得
        // return : https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos.html#_CPPv410eTaskState
        eTaskState status() const
        {
            return eTaskState(taskHandle_);
        }

        // 通知の送信
        // 参照 : https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos.html#_CPPv418xTaskGenericNotify12TaskHandle_t8uint32_t13eNotifyActionP8uint32_t
        // 前パラメータ省略するとxTaskNotifyGiveと同等になる
        // pulPreviousNotificationValueを省略するとxTaskNotifyと同等になる
        BaseType_t notify(uint32_t ulValue = 0, eNotifyAction eAction = eIncrement, uint32_t* pulPreviousNotificationValue = nullptr)
        {
            return xTaskGenericNotify(taskHandle_, ulValue, eAction, pulPreviousNotificationValue);
        }

        // 割り込みからの通知の送信
        // 参照 : https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos.html#_CPPv425xTaskGenericNotifyFromISR12TaskHandle_t8uint32_t13eNotifyActionP8uint32_tP10BaseType_t
        // pulPreviousNotificationValue : 省略可能
        BaseType_t notifyFromISR(uint32_t ulValue, eNotifyAction eAction, BaseType_t* pxHigherPriorityTaskWoken, uint32_t* pulPreviousNotificationValue = nullptr)
        {
            return xTaskGenericNotifyFromISR(taskHandle_, ulValue, eAction, pulPreviousNotificationValue, pxHigherPriorityTaskWoken);
        }

        // 通知の受け待ち
        // 参照 : https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos.html#_CPPv415xTaskNotifyWait8uint32_t8uint32_tP8uint32_t10TickType_t
        // pulNotificationValue : 参照OUT
        // ulBitsToClearOnEntry, ulBitsToClearOnExitを省略した場合entry/exit時のビットクリアはしない
        BaseType_t wait(uint32_t &pulNotificationValue, TickType_t xTicksToWait, uint32_t ulBitsToClearOnEntry = 0, uint32_t ulBitsToClearOnExit = 0)
        {
            return xTaskNotifyWait(ulBitsToClearOnEntry, ulBitsToClearOnExit, &pulNotificationValue, xTicksToWait);
        }

        // 全タスクの一時停止
        static void suspendAll()
        {
            vTaskSuspendAll();
        }

        // 全タスクの一時停止解除
        static void resumeAll()
        {
            xTaskResumeAll();
        }
    };

    // FreeRTOSのTask関連のラッパークラス
    // TaskControlの継承可能版
    // 参照 : https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos.html#task-api
    template <class T>
    class TaskControlV {
    private:
        TaskHandle_t taskHandle_;

        // 呼び出されたタスク
        // 中身でT::taskをすぐに呼び出すようにしている
        static IRAM_ATTR void task(void *p)
        {
            TaskControlV<T>* user = (TaskControlV<T>*)p;
            user->task();
            // タスクが終了したので削除処理を実施する。
            vTaskDelete(nullptr);
            user->taskHandle_ = nullptr;
        }

        // 実際に処理する内容は継承先で定義する
        virtual void task()
        {            
        }

    public:
        TaskControlV() : taskHandle_(nullptr) {}
        virtual ~TaskControlV() {}

        // プロセスの開始
        // taskName : タスク名
        // stackSize : タスクに割り当てるスタックサイズ
        // priority : 優先順位
        // core : 起動コア番号
        void startProcess(const char* taskName, uint32_t stackSize, UBaseType_t priority, BaseType_t core)
        {
            xTaskCreatePinnedToCore(TaskControlV::task, taskName,
                stackSize, (void*)this, priority, &taskHandle_, core);
        }

        // タスクの一時停止
        void suspend()
        {
            vTaskSuspend(taskHandle_);
        }

        // タスクの一時停止の再開
        void resume()
        {
            vTaskResume(taskHandle_);
        }

        // タスクの状態取得
        // return : https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos.html#_CPPv410eTaskState
        eTaskState status() const
        {
            return eTaskState(taskHandle_);
        }

        // 通知の送信
        // 参照 : https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos.html#_CPPv418xTaskGenericNotify12TaskHandle_t8uint32_t13eNotifyActionP8uint32_t
        // 前パラメータ省略するとxTaskNotifyGiveと同等になる
        // pulPreviousNotificationValueを省略するとxTaskNotifyと同等になる
        BaseType_t notify(uint32_t ulValue = 0, eNotifyAction eAction = eIncrement, uint32_t* pulPreviousNotificationValue = nullptr)
        {
            return xTaskGenericNotify(taskHandle_, ulValue, eAction, pulPreviousNotificationValue);
        }

        // 割り込みからの通知の送信
        // 参照 : https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos.html#_CPPv425xTaskGenericNotifyFromISR12TaskHandle_t8uint32_t13eNotifyActionP8uint32_tP10BaseType_t
        // pulPreviousNotificationValue : 省略可能
        BaseType_t notifyFromISR(uint32_t ulValue, eNotifyAction eAction, BaseType_t* pxHigherPriorityTaskWoken, uint32_t* pulPreviousNotificationValue = nullptr)
        {
            return xTaskGenericNotifyFromISR(taskHandle_, ulValue, eAction, pulPreviousNotificationValue, pxHigherPriorityTaskWoken);
        }

        // 通知の受け待ち
        // 参照 : https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos.html#_CPPv415xTaskNotifyWait8uint32_t8uint32_tP8uint32_t10TickType_t
        // pulNotificationValue : 参照OUT
        // ulBitsToClearOnEntry, ulBitsToClearOnExitを省略した場合entry/exit時のビットクリアはしない
        BaseType_t wait(uint32_t &pulNotificationValue, TickType_t xTicksToWait, uint32_t ulBitsToClearOnEntry = 0, uint32_t ulBitsToClearOnExit = 0)
        {
            return xTaskNotifyWait(ulBitsToClearOnEntry, ulBitsToClearOnExit, &pulNotificationValue, xTicksToWait);
        }

        // 全タスクの一時停止
        static void suspendAll()
        {
            vTaskSuspendAll();
        }

        // 全タスクの一時停止解除
        static void resumeAll()
        {
            xTaskResumeAll();
        }
    };
} // namespace Take4
