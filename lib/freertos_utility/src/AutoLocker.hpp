#pragma once
#include <esp32/rom/ets_sys.h>

namespace Take4
{
    // Tをロックオブジェクトとして関数から抜ける際に自動的にアンロックをするためのユーティリティクラス
    // コンストラクタ
    // target : ロック用オブジェクト
    // delayTime : ロックディレイをかける時間(単位:ms)
    template <typename T>
    class AutoLocker
    {
    private:
        T &target_;
        int delayTime_;

    public:
        AutoLocker(T &target, int delayTime = 0) : target_(target), delayTime_(delayTime)
        {
            target_.start();
        }

        ~AutoLocker()
        {
            target_.end();
            ets_delay_us(delayTime_ * 1000);
        }
    };
} // namespace Take4
