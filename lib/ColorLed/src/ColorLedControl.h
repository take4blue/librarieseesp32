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
        rmt_item32_t *sendBuffer_;      // 送信バッファ

        rmt_item32_t onoff[2];

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

        // LEDにデータを送信する
        // 事前にwait(false)をコールしてデータが送信済みかどうかを確認する
        void update();

        // LEDにデータが送信し終わったかどうかのチェック
        // checkOnly : falseの場合データ転送が完了するまで待ち続ける
        // return : trueの場合データ送信が完了したことを示す
        bool wait(bool checkOnly);
    };
} // namespace Take4
