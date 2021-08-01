#pragma once

namespace Take4
{
    // ストロボ制御用のインターフェース
    // 処理手順としては、setPower->ignite->detectEventで発光といった形になる
    // igniteとdetectEventで設定と実際の発光で分けているのは、設定と発光を別タスクで処理することを想定している。
    // 同一タスク内での処理の場合は、上の順番でコールするようにする
    class IStrobe
    {
    public:
        IStrobe();
        virtual ~IStrobe();

        // ストロボ出力値の設定
        // power : 発光出力値(0～21)
        // 1/1(0) --- 1/8(9) --- 1/32(15) --- 1/256(21)
        virtual void setPower(int power) = 0;
        
        // ストロボ出力値の取り出し
        // return : 現在設定されている発光出力値
        virtual int getPower() const = 0;

        // 次のdetectEventでストロボ発光を促す
        virtual void ignite() = 0;

        // ストロボ発光命令を検知して実際に発光させる
        virtual void detectEvent() = 0;
    };
    
} // namespace Take4
