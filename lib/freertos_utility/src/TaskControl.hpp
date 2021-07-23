#pragma once
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace Take4
{
    // タスクの制御を行うユーティリティクラス
    // Tクラスにはpublicでvoid task()というメソッドを用意しておくこと
    // 用意していないとコンパイルは通るがリンクでエラーになる
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
    };
} // namespace Take4
