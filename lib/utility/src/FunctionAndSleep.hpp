#pragma once
#if defined(ARDUINO_ARCH_AVR)
#include <arduino.h>
#define __UNO__
#include <ustd_platform.h>
#include <ustd_functional.h>
#elif defined(ESP_PLATFORM)
#include <esp_timer.h>
#include <unistd.h>
#include <functional>
#endif

namespace Take4 {
/// 機能の実施とSleepを組み合わせた物を連続実行するための仕組み
/// 機能とSleep時間はpush_backで追加する。
/// <>内の数は登録できる機能・Sleep時間の組み合わせの最大数
/// Heapは使っていない
template <const uint8_t MaxSet>
class FunctionAndSleep {
  public:
#if defined(ARDUINO_ARCH_AVR)
    typedef unsigned long Time_t;
    typedef ustd::function<void()> Function_t;
#elif defined(ESP_PLATFORM)
    typedef uint64_t Time_t;
    typedef std::function<void()> Function_t;
#endif

  private:
    /// 処理する機能とその後のSleep時間の組み合わせ
    struct FunctionSet {
        Function_t func_;
        Time_t time_;
    };
    /// 現在実行中のFunctionSetの位置
    uint8_t nState_;

    /// set_への格納数
    uint8_t nLast_;

    /// funcを実行した時の時間(ミリ秒)
    Time_t startTime_;

    /// 処理と時間の組み合わせ
    FunctionSet set_[MaxSet];

  public:
    FunctionAndSleep()
        : nState_(0)
        , nLast_(0)
    {
    }
    ~FunctionAndSleep() {}

    /// 機能とスリープ時間の組み合わせを追加する
    /// func : 処理する機能
    /// time : funcを実行したのちSleepする時間(単位:ミリ秒)
    void push_back(Function_t func, uint32_t time)
    {
        if (nLast_ >= MaxSet) {
            return;
        }
        set_[nLast_].func_ = func;
        set_[nLast_].time_ = time;
        ++nLast_;
    }

    /// 処理の開始
    void start()
    {
        nState_ = 0;
        set_[nState_].func_();
        startTime_ = millisec();
    }

    /// 処理の継続と最終まで処理したかの確認
    /// return : true=処理中、false=最後まで処理した
    bool doCheck()
    {
        if (nState_ >= nLast_) {
            return false;
        }
        auto current = millisec();
        if (current - startTime_ > set_[nState_].time_) {
            ++nState_;
            if (nState_ >= nLast_) {
                return false;
            }
            set_[nState_].func_();
            startTime_ = millisec();
        }
        return true;
    }

    /// ミリ秒を返す関数
    static Time_t millisec()
    {
#if defined(ARDUINO_ARCH_AVR)
        return millis();
#elif defined(ESP_PLATFORM)
        return esp_timer_get_time() / 1000;
#endif
    }
};
} // namespace Take4
