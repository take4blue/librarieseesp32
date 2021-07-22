#pragma once
#include "Magnetic.h"

namespace Take4 {
// 複数の磁気センサーのデータを取り扱うためのクラス
// データが変化したことを検知するための閾値と、閾値以内でさまよっている時間が長い場合に閾値を徐々に下げていく機能を付けている
// コンストラクタ
// threshhold : データが変化したことを検知するための閾値
// interval : 単位msで、ここで指定された時間閾値以内に納まっていると、判断する閾値を半分、半分と削っていく。
class Magnetics {
private:
    // 制御可能な最大の磁気センサー
    static const uint8_t maxData_ = 5;

    uint8_t nData_; // 取得対象のポートの数
    Magnetic data_[maxData_];
    uint8_t threshold_; // データが変化したことを示す閾値
    uint8_t position_; // 平均値を求めるためのvalue_のデータ位置
    uint32_t timer_;    // 閾値とともに使う変化しなかった時間(ミリ秒)
    uint32_t interval_; // 閾値に関しての時間経過による係数

public:
    Magnetics(uint8_t threshold = 10, uint32_t interval = 700);

    // 取得ポートの情報を追加
    bool add(Port_t channel);

    // 初期化
    void begin();

    // データ更新チェック
    // returnがtrueの場合、閾値を超えた変化があった場合
    bool check();

    // キャリブレーション
    // 指定した時間計測し、最大・最少の値を求める
    // timeは秒
    void calibration(uint8_t time);

    // データの取得
    int get(uint8_t no) const;

    // ポートの数
    uint8_t nPort() const;
};
}