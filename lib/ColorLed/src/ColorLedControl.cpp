#include "ColorLedControl.h"
#include "RMTLEDPattern.h"

namespace Take4
{   
    ColorLedControl::ColorLedControl(size_t nLed)
    : channel_(RMT_CHANNEL_0)
    , nLed_(nLed)
    , rgb_(nullptr)
    , sendBuffer_(nullptr)
    {
        onoff[0] = pl9823_3_bit0;
        onoff[1] = pl9823_3_bit1;
    }
    
    ColorLedControl::~ColorLedControl()
    {
        free(rgb_);
        free(sendBuffer_);
    }

    void ColorLedControl::begin(gpio_num_t pin, rmt_channel_t channel)
    {
        // RMTの初期設定
        channel_ = channel;
        rmt_config_t config = RMT_DEFAULT_CONFIG_TX(pin, channel_);
        config.clk_div = 8;     // 10Mhz設定
        config.mem_block_num = 1;
        ESP_ERROR_CHECK(rmt_config(&config));
        ESP_ERROR_CHECK(rmt_driver_install(channel_, 0, 0));

        rgb_ = (RGBBuffer_t*)calloc(sizeof(RGBBuffer_t) * BufferBlockSize, nLed_);
        // 送信バッファはnLed+1個確保
        // 最後は50μ秒のリセット情報を設定する
        sendBuffer_ = (rmt_item32_t*)calloc(sizeof(rmt_item32_t) * BufferBlockSize * 8, nLed_ + 1);
    }

    void ColorLedControl::set(size_t no, RGBBuffer_t r, RGBBuffer_t g, RGBBuffer_t b)
    {
        if (no < nLed_ && rgb_) {
            rgb_[no * BufferBlockSize + 0] = r;
            rgb_[no * BufferBlockSize + 1] = g;
            rgb_[no * BufferBlockSize + 2] = b;
        }
    }

    void ColorLedControl::update()
    {
        if (rgb_ && sendBuffer_) {
            size_t sPos = 0;
            for (size_t i = 0; i < nLed_ * BufferBlockSize; ++i) {
                for (size_t j = 0; j < 8; ++j) {
                    sendBuffer_[sPos] = onoff[((rgb_[i] >> (8 - j)) & 0x1 ? 1 : 0)];
                    ++sPos;
                }
            }
            sendBuffer_[sPos].duration0 = sendBuffer_[sPos].duration1 = 50;
            sendBuffer_[sPos].level0 = sendBuffer_[sPos].level1 = 0;
            ++sPos;
            rmt_write_items(channel_, sendBuffer_, sPos, false);
        }
    }

    bool ColorLedControl::wait(bool checkOnly)
    {
        esp_err_t err = ESP_OK;
        if (checkOnly) {
            err = rmt_wait_tx_done(channel_, 0);
        }
        else {
            do {
                err = rmt_wait_tx_done(channel_, 10);
            } while(err == ESP_ERR_TIMEOUT);
        }
        if (err == ESP_OK) {
            return true;
        }
        else {
            return false;
        }
    }

} // namespace Take4
