#include <memory.h>
#include <algorithm>
#include "ATM0130.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "AutoLocker.hpp"

using namespace Take4;

const uint8_t ATM0130::chars[475] = {
  0x00, 0x00, 0x00, 0x00, 0x00,  0x21, 0x08, 0x40, 0x10, 0x00,  0x52, 0x94, 0x00, 0x00, 0x00,  0x52, 0x95, 0xF5, 0x7D, 0x4A,  0x23, 0xE8, 0xE2, 0xF8, 0x80,  0xC6, 0x44, 0x44, 0x4C, 0x60,  0x64, 0xA8, 0x8A, 0xC9, 0xA0,  0x61, 0x10, 0x00, 0x00, 0x00,  0x11, 0x10, 0x84, 0x10, 0x40,  0x41, 0x04, 0x21, 0x11, 0x00,  0x01, 0x2A, 0xEA, 0x90, 0x00,  0x01, 0x08, 0xE2, 0x10, 0x00,  0x00, 0x00, 0x06, 0x11, 0x00,  0x00, 0x01, 0xF0, 0x00, 0x00,  0x00, 0x00, 0x00, 0x63, 0x00,  0x00, 0x44, 0x44, 0x40, 0x00,  0x74, 0x67, 0x5C, 0xC5, 0xC0,  0x23, 0x08, 0x42, 0x11, 0xC0,  0x74, 0x42, 0x22, 0x23, 0xE0,  0xF8, 0x88, 0x20, 0xC5, 0xC0,
  0x11, 0x95, 0x2F, 0x88, 0x40,  0xFC, 0x3C, 0x10, 0xC5, 0xC0,  0x32, 0x11, 0xE8, 0xC5, 0xC0,  0xF8, 0x44, 0x44, 0x21, 0x00,  0x74, 0x62, 0xE8, 0xC5, 0xC0,  0x74, 0x62, 0xF0, 0x89, 0x80,  0x03, 0x18, 0x06, 0x30, 0x00,  0x03, 0x18, 0x06, 0x11, 0x00,  0x11, 0x11, 0x04, 0x10, 0x40,  0x00, 0x3E, 0x0F, 0x80, 0x00,  0x41, 0x04, 0x11, 0x11, 0x00,  0x74, 0x42, 0x22, 0x00, 0x80,  0x74, 0x42, 0xDA, 0xD5, 0xC0,  0x74, 0x63, 0x1F, 0xC6, 0x20,  0xF4, 0x63, 0xE8, 0xC7, 0xC0,  0x74, 0x61, 0x08, 0x45, 0xC0,  0xE4, 0xA3, 0x18, 0xCB, 0x80,  0xFC, 0x21, 0xE8, 0x43, 0xE0,  0xFC, 0x21, 0xE8, 0x42, 0x00,  0x74, 0x61, 0x78, 0xC5, 0xE0,
  0x8C, 0x63, 0xF8, 0xC6, 0x20,  0x71, 0x08, 0x42, 0x11, 0xC0,  0x38, 0x84, 0x21, 0x49, 0x80,  0x8C, 0xA9, 0x8A, 0x4A, 0x20,  0x84, 0x21, 0x08, 0x43, 0xE0,  0x8E, 0xEB, 0x58, 0xC6, 0x20,  0x8C, 0x73, 0x59, 0xC6, 0x20,  0x74, 0x63, 0x18, 0xC5, 0xC0,  0xF4, 0x63, 0xE8, 0x42, 0x00,  0x74, 0x63, 0x1A, 0xC9, 0xA0,  0xF4, 0x63, 0xEA, 0x4A, 0x20,  0x74, 0x20, 0xE0, 0x87, 0xC0,  0xF9, 0x08, 0x42, 0x10, 0x80,  0x8C, 0x63, 0x18, 0xC5, 0xC0,  0x8C, 0x63, 0x18, 0xA8, 0x80,  0x8C, 0x63, 0x5A, 0xD5, 0x40,  0x8C, 0x54, 0x45, 0x46, 0x20,  0x8C, 0x62, 0xA2, 0x10, 0x80,  0xF8, 0x44, 0x44, 0x43, 0xE0,  0x72, 0x10, 0x84, 0x21, 0xC0,
  0x8A, 0xBE, 0x4F, 0x90, 0x80,  0x70, 0x84, 0x21, 0x09, 0xC0,  0x22, 0xA2, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x03, 0xE0,  0x41, 0x04, 0x00, 0x00, 0x00,  0x00, 0x1C, 0x17, 0xC5, 0xE0,  0x84, 0x2D, 0x98, 0xC7, 0xC0,  0x00, 0x1D, 0x08, 0x45, 0xC0,  0x08, 0x5B, 0x38, 0xC5, 0xE0,  0x00, 0x1D, 0x1F, 0xC1, 0xC0,  0x32, 0x51, 0xC4, 0x21, 0x00,  0x03, 0xE3, 0x17, 0x85, 0xC0,  0x84, 0x2D, 0x98, 0xC6, 0x20,  0x20, 0x18, 0x42, 0x11, 0xC0,  0x10, 0x0C, 0x21, 0x49, 0x80,  0x84, 0x25, 0x4C, 0x52, 0x40,  0x61, 0x08, 0x42, 0x11, 0xC0,  0x00, 0x35, 0x5A, 0xC6, 0x20,  0x00, 0x2D, 0x98, 0xC6, 0x20,  0x00, 0x1D, 0x18, 0xC5, 0xC0,
  0x00, 0x3D, 0x1F, 0x42, 0x00,  0x00, 0x1B, 0x37, 0x84, 0x20,  0x00, 0x2D, 0x98, 0x42, 0x00,  0x00, 0x1D, 0x07, 0x07, 0xC0,  0x42, 0x38, 0x84, 0x24, 0xC0,  0x00, 0x23, 0x18, 0xCD, 0xA0,  0x00, 0x23, 0x18, 0xA8, 0x80,  0x00, 0x23, 0x1A, 0xD5, 0x40,  0x00, 0x22, 0xA2, 0x2A, 0x20,  0x00, 0x23, 0x17, 0x85, 0xC0,  0x00, 0x3E, 0x22, 0x23, 0xE0,  0x11, 0x08, 0x82, 0x10, 0x40,  0x21, 0x08, 0x42, 0x10, 0x80,  0x41, 0x08, 0x22, 0x11, 0x00,  0x00, 0x11, 0x51, 0x00, 0x00,
};

ATM0130::ATM0130()
: figColor_(0)
, charFgColor_(0)
, charBgColor_(0)
, charX_(0)
, charY_(0)
, dcPin_(GPIO_NUM_NC)
, resetPin_(GPIO_NUM_NC)
, nQue_(0)
, posQue_(0)
, trans_(nullptr)
, index_ (0)
, pos_(0)
{
  setFigColor(0x0000);
  setCharColor(0xFFFF);
  setCharColorBG(0x00);
  memset(buffer_, 0, sizeof(buffer_));
}

static void transfer_callback(spi_transaction_t *t)
{
    if (t->user != nullptr) {
      gpio_set_level((gpio_num_t)((uint32_t)t->user & 0xFF), ((uint32_t)t->user & 0xFF00) != 0 ? 1 : 0);
    }
}

void ATM0130::writeReg(uint8_t data)
{
  if (nQue_ >= transSize_) {
    queResults();
  }
  auto index = posQue_ % transSize_;
  memset(trans_ + index, 0, sizeof(spi_transaction_t));
  trans_[index].flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA;
  trans_[index].length = 8;
  trans_[index].tx_data[0] = data;
  trans_[index].user = (void*)(dcPin_);
  auto ret = spi_device_queue_trans(device_, trans_ + index, portMAX_DELAY);
  assert(ret == ESP_OK);
  nQue_++;
  posQue_++;
}

void ATM0130::writeData(size_t len, uint8_t *buffer)
{
  if (len <= 4) {
    writeData(len, buffer[0], buffer[1], buffer[2], buffer[3]);
  }
  else {
    if (nQue_ >= transSize_) {
      queResults();
    }
    auto index = posQue_ % transSize_;
    memset(trans_ + index, 0, sizeof(spi_transaction_t));
    trans_[index].length = 8 * len;
    trans_[index].tx_buffer = buffer;
    trans_[index].user = (void*)(0x100 | dcPin_);
		auto ret = spi_device_queue_trans(device_, trans_ + index, portMAX_DELAY);
		assert(ret == ESP_OK);
    nQue_++;
    posQue_++;
  }
}

void ATM0130::writeData(size_t len, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4)
{
  if (nQue_ >= transSize_) {
    queResults();
  }
  if (len <= 4) {
    auto index = posQue_ % transSize_;
    memset(trans_ + index, 0, sizeof(spi_transaction_t));
    trans_[index].length = 8 * len;
    trans_[index].flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA;
    trans_[index].tx_data[0] = d1;
    trans_[index].tx_data[1] = d2;
    trans_[index].tx_data[2] = d3;
    trans_[index].tx_data[3] = d4;
    trans_[index].user = (void*)(0x100 | dcPin_);
		auto ret = spi_device_queue_trans(device_, trans_ + index, portMAX_DELAY);
		assert(ret == ESP_OK);
    nQue_++;
    posQue_++;
  }
}

void ATM0130::writeData(uint8_t data)
{
  writeData(1, data, 0, 0, 0);
}

void ATM0130::start()
{
  queResults();
  auto ret = spi_device_acquire_bus(device_, portMAX_DELAY);
  assert(ret == ESP_OK);
}

void ATM0130::queResults(int pos)
{
	spi_transaction_t* rtrans;
	esp_err_t ret;
	for (; nQue_ > pos; nQue_--) {
		ret = spi_device_get_trans_result(device_, &rtrans, portMAX_DELAY);
		assert(ret == ESP_OK);
	}
  if (pos == 0) {
    posQue_ = 0;
  }
}

void ATM0130::end()
{
  queResults();
  spi_device_release_bus(device_);
}

void ATM0130::begin(gpio_num_t dcPin, gpio_num_t resetPin, int mosi, int miso, int sclk, int cs, int freq)
{
  dcPin_ = dcPin;
  resetPin_ = resetPin;
  gpio_config_t io_conf = {
      1ULL << dcPin_ | 1ULL << resetPin_,
      GPIO_MODE_OUTPUT,
      GPIO_PULLUP_DISABLE,
      GPIO_PULLDOWN_DISABLE,
      GPIO_INTR_DISABLE
  };
  gpio_config(&io_conf);

  resetLCD();
  // SPIの初期化
  spi_bus_config_t buscfg = { 
    .mosi_io_num = mosi,
    .miso_io_num = miso,
    .sclk_io_num = sclk, 
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
    .max_transfer_sz = sizeof(uint16_t) * bufferSize_,
    .flags = SPICOMMON_BUSFLAG_MASTER | SPICOMMON_BUSFLAG_SCLK | SPICOMMON_BUSFLAG_MOSI,
    .intr_flags = 0
  };

  esp_err_t ret = spi_bus_initialize(VSPI_HOST, &buscfg, 2);
  assert(ret == ESP_OK);

  auto freqw = spi_get_actual_clock(APB_CLK_FREQ, freq, 0);
  spi_device_interface_config_t devcfg = {
      .command_bits = 0, .address_bits = 0, .dummy_bits = 0,
      .mode = 0, // SPI mode
      .duty_cycle_pos = 0, .cs_ena_pretrans = 0, .cs_ena_posttrans = 0,
      .clock_speed_hz = freqw,
      .input_delay_ns = 0,
      .spics_io_num = cs, //CS pin
      .flags = 0,
      .queue_size = transSize_, //We want to be able to queue 7 transactions at a time
      .pre_cb = transfer_callback,
      .post_cb = nullptr
  };
  ret = spi_bus_add_device(VSPI_HOST, &devcfg, &device_);
  trans_ = (spi_transaction_t*)heap_caps_malloc(transSize_ * sizeof(spi_transaction_t), MALLOC_CAP_DEFAULT);
  for (int i = 0; i < bufferNum_; i++) {
    buffer_[i] = (uint16_t*)heap_caps_malloc(bufferSize_ * sizeof(uint16_t), MALLOC_CAP_DMA);
  }

  {
    AutoLocker<ATM0130> lock(*this, 100);
    writeReg(0x11);
  }
  {
    AutoLocker<ATM0130> lock(*this, 2);
    writeReg(0x36);  //MADCTL
    writeData(0x00);
    //MY=0
    //MX=0
    //MV=0
    //ML=0
    //RGB=0
    //MH=0
    writeReg(0x3A);
    writeData(0x55); //65K color , 16bit / pixel

    ////--------------------------------ST7789V Frame rate
    writeReg(0xb2);
    writeData(4, 0x0c, 0x0c, 0x0c, 0x33);
    writeData(0x33);
  }
  {
    AutoLocker<ATM0130> lock(*this, 2);
    writeReg(0xb7);
    writeData(0x75);
  }
  ////---------------------------------ST7789V Power
  {
    AutoLocker<ATM0130> lock(*this, 2);
    writeReg(0xc2);
    writeData(0x01);
  }
  {
    AutoLocker<ATM0130> lock(*this, 2);
    writeReg(0xc3);
    writeData(0x10);
  }
  {
    AutoLocker<ATM0130> lock(*this, 2);
    writeReg(0xc4);
    writeData(0x20);
  }
  {
    AutoLocker<ATM0130> lock(*this, 2);
    writeReg(0xc6);
    writeData(0x0f);
    writeReg(0xb0);
    writeData(2, 0x00, 0xf0, 0, 0);//RRRR RGGGG GGGB BBBB
  }
  {
    AutoLocker<ATM0130> lock(*this, 2);
    writeReg(0xD0);
    writeData(2, 0xA4, 0xA1, 0, 0);
  }
  ////--------------------------------ST7789V gamma
  {
    AutoLocker<ATM0130> lock(*this, 2);
    writeReg(0x21);
  }
  {
    AutoLocker<ATM0130> lock(*this, 2);
    writeReg(0xbb);
    writeData(0x3b);
  }
  {
    AutoLocker<ATM0130> lock(*this, 2);
    writeReg(0xE0);    //Set Gamma
    writeData(4, 0xF0, 0x0b, 0x11, 0x0e);
    writeData(4, 0x0d, 0x19, 0x36, 0x33);
    writeData(4, 0x4b, 0x07, 0x14, 0x14);
    writeData(2, 0x2c, 0x2e, 0, 0);
  }
  {
    AutoLocker<ATM0130> lock(*this, 2);
    writeReg(0xE1);    //Set Gamma
    writeData(4, 0xF0, 0x0d, 0x12, 0x0b);
    writeData(4, 0x09, 0x03, 0x32, 0x44);
    writeData(4, 0x48, 0x39, 0x16, 0x16);
    writeData(2, 0x2d, 0x30, 0, 0);
    writeReg(0x2A);
    writeData(4, 0x00, 0x00, 0x00, 0xEF);
    writeReg(0x2B);
    writeData(4, 0x00, 0x00, 0x00, 0xEF);
    writeReg(0x29);    //Display on
  }
  {
    AutoLocker<ATM0130> lock(*this);
    writeReg(0x2c);
  }
}

void ATM0130::setFigColor(uint8_t r, uint8_t g, uint8_t b)
{
  figColor_ = convRGB(r, g, b);
}

void ATM0130::setFigColor(uint16_t c)
{
  figColor_ = c;
}

void ATM0130::setCharPlace(uint8_t x, uint8_t y)
{
  charX_ = x;
  charY_ = y;
}

void ATM0130::setCharColor(uint8_t r, uint8_t g, uint8_t b)
{
  charFgColor_ = convRGB(r, g, b);
}

void ATM0130::setCharColor(uint16_t c)
{
  charFgColor_ = c;
}

void ATM0130::setCharColorBG(uint8_t r, uint8_t g, uint8_t b)
{
  charBgColor_ = convRGB(r, g, b);
}

void ATM0130::setCharColorBG(uint16_t c)
{
  charBgColor_ = c;
}

void ATM0130::print(char ch)
{
  if (charX_ > 235) {
    charX_ = 0;
    charY_ += 8;
  }
  if (charY_ > 232) {
    charX_ = 0;
    charY_ = 0;
  }

  if (ch == '\n') {
    charX_ = 0;
    charY_ += 8;
  }
  else {
    setCharQueue(ch);
    writeCharQueue();
    charX_ += 6;
  }
}

void ATM0130::print(const std::string& str)
{
  std::for_each(str.begin(), str.end(), [this](char x){print(x);});
}

void ATM0130::resetLCD(void)
{
  gpio_set_level(resetPin_, 1);
  vTaskDelay(20/portTICK_PERIOD_MS);
  gpio_set_level(resetPin_, 0);
  vTaskDelay(20/portTICK_PERIOD_MS);
  gpio_set_level(resetPin_, 1);
  vTaskDelay(20/portTICK_PERIOD_MS);
}

void ATM0130::setWindow(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
  writeReg(0x2A);
  writeData(4, 0x00, x, 0x00, x + width - 1);
  writeReg(0x2B);
  writeData(4, 0x00, y, 0x00, y + height - 1);
  writeReg(0x2c);
  index_ = pos_ = 0;
}

void ATM0130::putPixel(uint16_t color)
{
  auto buffer = buffer_[index_ % bufferNum_];
  buffer[pos_] = color;
  pos_++;
  if (pos_ % bufferSize_ == 0) {
    queResults(bufferNum_ - 2);
    writeData(bufferSize_ * sizeof(uint16_t), (uint8_t*)buffer);
    pos_ = 0;
    index_++;
  }
}

void ATM0130::endWindow()
{
  if (pos_ != 0) {
    auto buffer = buffer_[index_ % bufferNum_];
    queResults(bufferNum_ - 2);
    writeData(pos_ * sizeof(uint16_t), (uint8_t*)buffer);
  }
}

void ATM0130::drawRectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
  int loop = width * height;
  AutoLocker<ATM0130> lock(*this);
  setWindow(x, y, width, height);
  for (int i = 0; i < loop; i++) {
    putPixel(figColor_);
  }
  endWindow();
}

void ATM0130::setCharQueue(uint8_t c)
{
  if ((c >= 0x20) && (c <= 0x7E)) {
    c -= 0x20;
    for (uint8_t i = 0; i < 5; i++) {
      char_queue[i] = chars[5 * c + i];
    }
  }
  else {
    for (uint8_t i = 0; i < 5; i++) {
      char_queue [i] = 0xFF;
    }
  }
}

void ATM0130::writeCharQueue()
{
  AutoLocker<ATM0130> lock(*this);
  int counter = 0;
  setWindow(charX_, charY_, 6, 8);
  for (uint8_t i = 0; i < 5; i++) {
    for (uint8_t j = 0; j < 8; j++, counter++) {
      if ((char_queue[i] & (0x80 >> j)) > 0) {
        putPixel(charFgColor_);
      }
      else {
        putPixel(charBgColor_);
      }
      if (counter % 5 == 4) {
        putPixel(charBgColor_);
      }
    }
  }
  endWindow();
}

uint16_t ATM0130::convRGB(uint8_t red, uint8_t green, uint8_t blue)
{
  uint16_t color = 0;
  color = blue >> 3;
  color |= ((green & 0xFC) << 3);
  color |= ((red & 0xF8) << 8);
  return color;
}
