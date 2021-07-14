#pragma once
#if defined(ARDUINO_ARCH_AVR)
#include <IRin.h>
#include <arduino.h>

namespace RecordIr {
// アプリケーションクラス
// テンプレート引数で、登録可能なIRコードの数を指定する
template <const uint8_t maxIr_>
class App {
private:
    Take4::IRin ii_;
    byte state_;
    uint8_t nIr_;
    Take4::IRData irCode_[maxIr_]; // 登録されているIRコード

public:
    App()
        : ii_()
        , state_(0)
        , nIr_()
    {
        for (uint8_t i = 0; i < maxIr_; i++) {
            irCode_[i].formatType = Take4::NOFormat;
        }
    }

    void begin(PortNo_t pin)
    {
        ii_.begin(pin);
        Serial.begin(9600);
        Serial.println("start");
    }

    // リモコンコードの登録
    void record()
    {
        for (nIr_ = 0; nIr_ < maxIr_; nIr_++) {
            while (!ii_.parse() || ii_.formatType() == Take4::NOFormat || ii_.isRepeat())
                ;
            if (nIr_ != 0 && irCode_[0].isEqual(ii_.data())) {
                // 0番目と同じキーが入力された、登録はここまで
                return;
            }
            irCode_[nIr_] = ii_.data();
        }
    }

    void task()
    {
        if (Serial.available()) {
            byte cmd = Serial.read();
            switch (cmd) {
            case 'r':
                state_ = 1;
                Serial.println("RK");
                break;
            }
        }
        switch (state_) {
        case 1:
            // リモコンコードの記録
            record();
            Serial.print("EK ");
            Serial.println(nIr_);
            state_ = 0;
            break;
        default:
            // リモコンコードに対しての処理
            if (ii_.parse()) {
                // キーが押された場合
                for (uint8_t i = 0; i < nIr_; i++) {
                    if (ii_.data().isEqual(irCode_[i])) {
                        Serial.print("Push ");
                        Serial.println(i);
                        break;
                    }
                }
            }
            break;
        }
    }
};
}
#endif