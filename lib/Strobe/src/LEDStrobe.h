#pragma once
#include "IStrobe.h"
#include <driver/gpio.h>
#include "driver/ledc.h"

namespace Take4
{
    // 可視光LEDで外部ストロボを制御する
    // 動作対象はSea&SeaやINONの光接続水中ストロボ
    // 処理手順はIStrobe側を参照。
    class LEDStrobe : public IStrobe
    {
    private:
        gpio_num_t led_;
        volatile int power_;
        volatile bool igniteStrobe_;
        const uint16_t* flashTime_;

    public:
        LEDStrobe();
        virtual ~LEDStrobe();

        // ピンの初期化
        // led : 制御用ピン
        void begin(gpio_num_t led);

        // ストロボ出力値の設定
        // power : 発光出力値(0～21)
        // 1/1(0) --- 1/8(9) --- 1/32(15) --- 1/256(21)
        virtual void setPower(int power);

        // ストロボ出力値の取り出し
        // return : 現在設定されている発光出力値
        virtual int getPower() const;

        // 次のdetectEventでフラッシュ発光を促す
        virtual void ignite();

        // フラッシュ発光命令を検知して実際に発光させる
        virtual void detectEvent();

        // 発光のための調整時間の設定
        // values : 発光時間をμ秒で設定した配列22個分。nullptrを指定した場合デフォルトの値に戻す
        void setFlashTime(const uint16_t* values);
    };
} // namespace Take4
