#pragma once
#include "IRCommon.h"

namespace Take4 {
// 赤外線リモコンの読み込み・解析用クラス
// 割り込み復帰直後からの直解析は、うまくいかない。
// IRでの割り込み復帰後は、500ms位あとからparseでの解析処理を実施するようにする
// delayのためESP_PLATFORMの場合1tick1msが望ましい。
// ARDUINO_ARCH_AVR, ESP_PLATFORM
class IRin {
private:
#if defined(ARDUINO_ARCH_AVR)
    PortNo_t pin_;      // IRが使用しているポート番号
    uint32_t timeOut_; // Frame開始を検知するためのタイムアウト時間(マイクロ秒)
#endif
    IRData data_;   // 解析データ
#if defined(ESP_PLATFORM)
    uint8_t channel_;
    void* ringBuffer_;
#endif

public:
	// コンストラクタ
	// pin : 接続されているピン(Dポートのみ対応なので0～7)
	IRin();
    ~IRin();

    // begin内でポートの使用宣言をする
	// pin : 接続されているピン(GPIO番号で0～39)、ESP_PLATFORMのみ追加で、channel : RMT機能のチャンネル番号(0～7)
    void begin(PortNo_t pin
#if defined(ESP_PLATFORM)
        , uint8_t channel
#endif
    );

    // IRからのデータ受信とコマンド解析
    // returnがtrueの場合、正しい解析結果が得られた。
    // または、前回の状態から変化があった場合(ボタンOFF)
    // falseの場合、解析結果が得られないか、信号の受信ができなかった場合
    bool parse();

    // 解析結果のフォーマット種別の取り出し
    IRFormatType formatType() const;

    // 解析データの取り出し
    const IRData& data() const;

    // リピート状態
    bool isRepeat() const;
};
}