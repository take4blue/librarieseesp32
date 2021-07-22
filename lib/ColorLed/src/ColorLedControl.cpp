#include "ColorLedControl.h"
#include "RMTLEDPattern.h"

namespace Take4
{   
    ColorLedControl::ColorLedControl(size_t nLed)
    : channel_(RMT_CHANNEL_0)
    , nLed_(nLed)
    , rgb_(nullptr)
    , reset_(0)
    , counter_(0)
    {
        onoff[0].level0 = onoff[1].level0 = 1;
        onoff[0].level1 = onoff[1].level1 = 0;
        sendBuffer_[0] = sendBuffer_[1] = nullptr;
    }
    
    ColorLedControl::~ColorLedControl()
    {
        free(rgb_);
        free(sendBuffer_[0]);
        free(sendBuffer_[1]);
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
        sendBuffer_[0] = (rmt_item32_t*)calloc(sizeof(rmt_item32_t) * BufferBlockSize * 8, nLed_ + 1);
        sendBuffer_[1] = (rmt_item32_t*)calloc(sizeof(rmt_item32_t) * BufferBlockSize * 8, nLed_ + 1);

        setTransmissionTime(4, 8, 8, 4, 500);
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
        ++counter_;
        if (rgb_ && sendBuffer_[counter_ % 2]) {
            size_t sPos = 0;
            for (size_t i = 0; i < nLed_ * BufferBlockSize; ++i) {
                for (size_t j = 0; j < 8; ++j) {
                    sendBuffer_[counter_ % 2][sPos] = onoff[((rgb_[i] >> (8 - j)) & 0x1 ? 1 : 0)];
                    ++sPos;
                }
            }
            if (reset_ > 0) {
                sendBuffer_[counter_ % 2][sPos].duration0 = reset_;
                sendBuffer_[counter_ % 2][sPos].duration1 = 0;
                sendBuffer_[counter_ % 2][sPos].level0 = sendBuffer_[counter_ % 2][sPos].level1 = 0;
                ++sPos;
            }
            wait(false);
            rmt_write_items(channel_, sendBuffer_[counter_ % 2], sPos, false);
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

    void ColorLedControl::setTransmissionTime(uint16_t t0h, uint16_t t0l, uint16_t t1h, uint16_t t1l, uint16_t reset)
    {
        onoff[0].duration0 = t0h;
        onoff[0].duration1 = t0l;
        onoff[1].duration0 = t1h;
        onoff[1].duration1 = t1l;
        reset_ = reset;
    }

    void ColorLedControl::setTransmissionTime(uint16_t reset)
    {
        reset_ = reset;
    }

} // namespace Take4
