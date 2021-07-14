#pragma once
#if defined(ARDUINO)
#include <arduino.h>
#endif
#if defined(ESP_PLATFORM)
#include <driver/adc.h>
typedef adc1_channel_t Port_t;
#else
typedef uint8_t Port_t;
#endif

namespace Take4 {
// 1磁気センサーについての情報管理
// sum_, prevValue_, value_でノイズフィルター的な役割を与えている
class Magnetic {
private:
    static const uint8_t maxOriginRetry_ = 20;
    static const uint8_t maxGetRetry_ = 15;

    uint16_t origin_; // 安定時のポートの値：原点値
    uint16_t sum_; // 平均値を求めるためのmaxGetRetry_の取得値の合計値
    uint16_t prevValue_; // 前回の値
    uint16_t current_;  // 現在の値
    uint16_t value_[maxGetRetry_]; // 平均値を求めるための基礎データ
    Port_t channel_; // 取得対象のポート情報
    uint16_t max_;  // 最大の磁気値
    uint16_t min_;  // 最低の磁気値
    uint16_t indexThreshold_;   // この磁気センサー個別の閾値の上乗せ値

    // 磁界情報をセンサーから取得する
    int sensorValue() const;

public:
    // コンストラクタ
    Magnetic();

    // 初期化処理
    void begin();

    // アナログ入力ポートの設定
    void set(Port_t val);

    void calibration();

    // 現在の値を取り出す
    int get() const;

    // 磁気情報を読み込み、前回から変化があった場合valueを更新する
    // returnがtrueの場合、変化があった。
    bool check(uint8_t position, uint8_t threshold);

    static uint8_t retryNum();
};
}