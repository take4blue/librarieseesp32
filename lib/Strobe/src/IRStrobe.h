#pragma once
#include <driver/gpio.h>
#include <driver/rmt.h>
#include "IStrobe.h"

namespace Take4
{
    // 赤外線でEOSストロボを制御するクラス
    // 処理手順はIStrobe側を参照。
    class IRStrobe : public IStrobe {
    public:
        // 発光種類
        enum StrobeType {
            Manual,             // マニュアル発光
            ETTLPreStrobe,      // ETTLのプリ発光
            ETTLStrobe,         // ETTLの本発光
        };
    
    private:
        rmt_channel_t channel_;

        volatile int value_;
        volatile StrobeType type_;        
        volatile bool igniteStrobe_;

    public:
        IRStrobe();

        virtual ~IRStrobe();

        // 初期化関数
        // pin : 制御ピン
        // channel : RMTのチャンネル番号
        void begin(gpio_num_t pin, rmt_channel_t channel);

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

        // 発光種類の設定
        // type : 発光種類
        void set(StrobeType type);

        // 指定されたパラメータでIRLEDを発光し外部ストロボを制御する
        void sendParameter() const;
    };
} // namespace Take4
