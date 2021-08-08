#pragma once
#include <esp32/rom/ets_sys.h>

namespace Take4 {
// Tをロックオブジェクトとして関数から抜ける際に自動的にアンロックをするためのユーティリティクラス
// std::lock_guardと同じだが、unlock後の待機時間を設定できる。
// コンストラクタ
// target : ロック用オブジェクト
// delayTime : ロックディレイをかける時間(単位:ms)
template <typename T>
class LockGuard {
  private:
    T &target_;
    int delayTime_;

  public:
    LockGuard(T &target, int delayTime = 0)
        : target_(target)
        , delayTime_(delayTime)
    {
        target_.lock();
    }

    ~LockGuard()
    {
        target_.unlock();
        ets_delay_us(delayTime_ * 1000);
    }

    LockGuard(const LockGuard &) = delete;
    LockGuard &operator=(const LockGuard &) = delete;
};
} // namespace Take4
