#pragma once
#include <driver/rmt.h>

namespace Take4
{
    // フルカラーLEDの色制御用RMTデータパターン
    // CLK分割数は10Mhzにするので100ns単位でrmt_item32_tの設定をする

    // データシートから類推した値
    const rmt_item32_t ws2311bit0 = {{{5, 1, 20, 0}}};
    const rmt_item32_t ws2311bit1 = {{{12, 1, 13, 0}}};

    // データシートから類推した値
    const rmt_item32_t pl9823_1_bit0 = {{{3, 1, 14, 0}}};
    const rmt_item32_t pl9823_1_bit1 = {{{14, 1, 3, 0}}};

    // 以下の設定は http://www.riric.jp/electronics/AVR/tech/PL9823(WS2811).html を参照
    const rmt_item32_t pl9823_2_bit0 = {{{4, 1, 10, 0}}};
    const rmt_item32_t pl9823_2_bit1 = {{{9, 1, 5, 0}}};

    // pl9823_1_bitで出力した時pl9823が出したタイミングで800kHzに近いタイミングに調整してみた。
    const rmt_item32_t pl9823_3_bit0 = {{{4, 1, 8, 0}}};
    const rmt_item32_t pl9823_3_bit1 = {{{8, 1, 4, 0}}};
} // namespace Take4
