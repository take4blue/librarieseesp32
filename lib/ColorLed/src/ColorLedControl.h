#pragma once
#include <driver/rmt.h>
#include <stdint.h>

namespace Take4
{
    typedef uint8_t RGBBuffer_t;

    // 複数個数のフルカラーLED制御用クラス
    // コンストラクタで制御を行うLEDの数を指定する
    class ColorLedControl
    {
    private:
        // RGBバッファの1データのサイズ
        // RGBBuffer_tをBufferBlockSize * updateのnum分用意しておく
        static const size_t BufferBlockSize = 3;

        rmt_channel_t channel_;

        size_t nLed_;
        RGBBuffer_t *rgb_;              // 色の格納場所
        rmt_item32_t *sendBuffer_[2];      // 送信バッファ

        rmt_item32_t onoff[2];
        uint16_t reset_;
        uint8_t counter_;

    public:
        // コンストラクタ
        // nLed : LEDの数
        ColorLedControl(size_t nLed);

        ~ColorLedControl();

        // 初期処理
        // pin : LEDを接続するピン番号
        // channel : RMT制御用のチャンネル番号
        void begin(gpio_num_t pin, rmt_channel_t channel);

        // 色データの設定
        // no : 設定するLEDの場所(最大はコンストラクタでnLed未満)
        // rgb : 色
        void set(size_t no, RGBBuffer_t r, RGBBuffer_t g, RGBBuffer_t b);

        // LEDにデータをウェイト無しで送信
        // 内部でデータ転送を行う前にはwait(false)で前回の送信が完了するまで待つようにはしている
        // 送信データのバッティングが起きないようにダブルバッファにしている
        // ただしスレッド等で使う場合update呼び出しのタイミングでset系の呼び出しを行わないように調整をする必要がある
        void update();

        // LEDにデータが送信し終わったかどうかのチェック
        // checkOnly : falseの場合データ転送が完了するまで待ち続ける
        // return : trueの場合データ送信が完了したことを示す
        bool wait(bool checkOnly);

        // 転送時間の設定を行う
        // t0h, t0l, t1h, t1l, reset : それぞれの時間を100nsを1と換算し設定する。
        // ただreset時間は最短設定どおりにはならない。
        // デフォルト(旧WS2812B) : 4, 8, 8, 4, 500
        // SK6812MINI : 3, 9, 6, 6, 800
        void setTransmissionTime(uint16_t t0h, uint16_t t0l, uint16_t t1h, uint16_t t1l, uint16_t reset);

        // 転送時のリセット時間を設定する
        // ただreset時間は最短設定どおりにはならないのでロジアナ等で計測してから最適値を設定することをお勧めする。
        // reset : それぞれの時間を100nsを1と換算し設定する。
        void setTransmissionTime(uint16_t reset);
    };
} // namespace Take4
