#pragma once
#include <driver/rmt.h>

namespace Take4
{
    typedef uint8_t RGBBuffer_t;

    // PL9823フルカラーLED制御用クラス
    class ColorLed
    {
    private:
        rmt_channel_t channel_;

        // 転送データ用コールバック
        static void u8toRmt(const void* src, rmt_item32_t* dest, size_t src_size,
    size_t wanted_num, size_t* translated_size, size_t* item_num);

    public:
        // RGBバッファの1データのサイズ
        // RGBBuffer_tをBufferBlockSize * updateのnum分用意しておく
        static const size_t BufferBlockSize = 3;

        ColorLed();
        ~ColorLed();

        // 初期処理
        // pin : LEDを接続するピン番号
        // channel : RMT制御用のチャンネル番号
        void begin(gpio_num_t pin, rmt_channel_t channel);

        // LEDの更新
        // rgb : RGBの色を各色1バイト、合計3バイトで表現する。rgbのバイト数はnum*3必要
        // num : 変更したいLEDの数、1以上
        void update(const RGBBuffer_t *rgb, size_t num);
    };
} // namespace Take4
