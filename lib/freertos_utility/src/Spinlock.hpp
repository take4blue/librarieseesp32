#pragma once
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <soc/spinlock.h>

namespace Take4 {
// spinlockのラッパークラスでタスク内のクリティカルセクションを指示するために使う
// 実際にはSpinlockとstd::lock_guardを組み合わせ、クリティカルセクションをブロック{}でくくり管理する
// spinlock関数のラッピングもしている
// ISR系も今のところこれを使っても大丈夫みたい。どちらもportENTER_CRITICALへの置換みたいなので。(FreeRTOS Kernel V10.2.1のポーティングでは)
class Spinlock {
  private:
    spinlock_t lock_;

  public:
    Spinlock()
    {
        spinlock_initialize(&lock_);
    }
    ~Spinlock() {}

    void lock()
    {
        taskENTER_CRITICAL(&lock_);
    }

    void unlock()
    {
        taskEXIT_CRITICAL(&lock_);
    }

    // Top level spinlock acquire function, spins until get the lock
    // timeout : cycles to wait, passing SPINLOCK_WAIT_FOREVER blocs indefinitely
    bool acquire(int32_t timeout)
    {
        return spinlock_acquire(&lock_, timeout);
    }

    // ロックを試みる
    // タイムアウト時間0でacquireを呼び出す
    // return : ロックが成功したらtrue
    bool try_lock()
    {
        return acquire(0);
    }

    // Top level spinlock unlock function, unlocks a previously locked spinlock
    void release()
    {
        spinlock_release(&lock_);
    }

    Spinlock(const Spinlock &) = delete;
    Spinlock &operator=(const Spinlock &) = delete;
};
} // namespace Take4
