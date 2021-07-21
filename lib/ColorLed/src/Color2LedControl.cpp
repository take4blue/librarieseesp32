#include "Color2LedControl.h"
#include <driver/gpio.h>
#include <esp32/rom/ets_sys.h>
#include <algorithm>

namespace Take4
{
    Color2LedControl::Color2LedControl()
        : channel_(RMT_CHANNEL_0)
    {
    }

    Color2LedControl::~Color2LedControl()
    {
    }
    // CLK分割数は10Mhzにするので100ns単位でrmt_item32_tの設定をする

    // データシートから類推した値
    static const rmt_item32_t ws2311bit0 = {{{5, 1, 20, 0}}};
    static const rmt_item32_t ws2311bit1 = {{{12, 1, 13, 0}}};

    // データシートから類推した値
    static const rmt_item32_t pl9823_1_bit0 = {{{3, 1, 14, 0}}};
    static const rmt_item32_t pl9823_1_bit1 = {{{14, 1, 3, 0}}};

    // 以下の設定は http://www.riric.jp/electronics/AVR/tech/PL9823(WS2811).html を参照
    static const rmt_item32_t pl9823_2_bit0 = {{{4, 1, 10, 0}}};
    static const rmt_item32_t pl9823_2_bit1 = {{{9, 1, 5, 0}}};

    // pl9823_1_bitで出力した時pl9823が出したタイミングで800kHzに近いタイミングに調整してみた。
    static const rmt_item32_t pl9823_3_bit0 = {{{4, 1, 8, 0}}};
    static const rmt_item32_t pl9823_3_bit1 = {{{8, 1, 4, 0}}};

    void IRAM_ATTR Color2LedControl::u8toRmt(const void *src, rmt_item32_t *dest, size_t src_size,
                                     size_t wanted_num, size_t *translated_size, size_t *item_num)
    {
        // データチェック
        if (src == nullptr || dest == nullptr) {
            *translated_size = 0;
            *item_num = 0;
            return;
        }

        size_t size = 0;
        size_t num = 0;
        uint8_t *psrc = (uint8_t *)src; // RGBバッファ情報に変換
        // 転送データ数を決定する
        size_t transferNum = std::min(wanted_num / 24, src_size / 3);       // RGBのビット数24ビットで割り、いくつのLED用にデータを送信するかを決める

        for (rmt_item32_t *pdest = dest; size < (transferNum * 3) && num < wanted_num; ++size) {
            for (int i = 7; i >= 0; --i, ++num) {
                if ((psrc[size] >> i) & 0x1) {
                    pdest[num].val = pl9823_3_bit1.val;
                }
                else {
                    pdest[num].val = pl9823_3_bit0.val;
                }
            }
        }
        *translated_size = size;
        *item_num = num;
    }

    void Color2LedControl::begin(gpio_num_t pin, rmt_channel_t channel)
    {
        gpio_config_t gpio = {
            .pin_bit_mask = 1UL << pin,
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
        };
        gpio_config(&gpio);
        // ここでリセット
        gpio_set_level(pin, 0);
        ets_delay_us(50);

        // RMTの初期設定
        channel_ = channel;
        rmt_config_t config = RMT_DEFAULT_CONFIG_TX(pin, channel_);
        config.clk_div = 8;     // 10Mhz設定
        config.mem_block_num = 1;
        ESP_ERROR_CHECK(rmt_config(&config));
        ESP_ERROR_CHECK(rmt_driver_install(channel_, 0, 0));

        ESP_ERROR_CHECK(rmt_translator_init(channel_, Color2LedControl::u8toRmt));
    }

    void Color2LedControl::update(const RGBBuffer_t *rgb, size_t num)
    {
        if (rgb != nullptr && num > 0) {
            ESP_ERROR_CHECK(rmt_write_sample(channel_, rgb, num * BufferBlockSize, true));
        }
    }
} // namespace Take4
