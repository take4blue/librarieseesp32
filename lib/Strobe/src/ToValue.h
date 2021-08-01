#pragma once

namespace Take4
{
    namespace Canon
    {
        // ストロボ強度への変換
        // value : ETTLでストロボ通信に出てくる強度値、
        // return : 0からの順番になっている光量の強度数値。1/1:0 ～ 1/64:18 -1はvalueがおかしい値の場合
        int IntToStrobePower(int value);

        // ストロボ強度への変換
        // value : 0からの順番になっている光量の強度数値。1/1:0 ～ 1/64:18
        // return : ETTLでストロボ通信に出てくる強度値
        int StrobePowerToInt(int value);
    } // namespace Canon
} // namespace Take4
