#include <driver/spi_master.h>
#include <stdint.h>
#include <string>
#include <driver/gpio.h>

namespace Take4
{
  // 液晶 ATM0130B3 制御クラス
  // もともとは秋月の販売サイトにあったArduino用のもので、それをESP32用に変更したもの
  class ATM0130
  {
  private:
    spi_device_handle_t device_;

    static const int transSize_ = 7;
    static const int bufferSize_ = 512; // 転送用バッファの大きさ
    static const int bufferNum_ = 3;    // 転送用バッファの数:DMA転送を行うため、転送中に送信データを作成できるようにするため複数バッファを保持する
    static const int freq_ = SPI_MASTER_FREQ_40M;

    uint16_t figColor_;       // 描画色
    uint16_t charFgColor_;    // 文字描画色
    uint16_t charBgColor_;    // 文字背景色
    uint8_t charX_;           // 文字描画開始X
    uint8_t charY_;           // 文字描画開始Y
    gpio_num_t dcPin_;        // Data/Command指定ピン
    gpio_num_t resetPin_;     // リセットピン

    uint8_t nQue_;            // DMA転送用バッファの数
    uint8_t posQue_;          // DMAでどのバッファが転送されているか
    spi_transaction_t *trans_;
    uint16_t *buffer_[bufferNum_];    // 転送用バッファ
    int index_;               // posQue_と組み合わせて使用
    int pos_;                 // バッファ内のデータ格納末尾

  public:
    ATM0130();

    // IO情報の設定と初期化
    // dcPin : data command選択ピン
    // resetPin : リセット
    // mosi, miso, sclk, cs : SPIピン
    // freq : 転送周波数(Hz)
    void begin(gpio_num_t dcPin, gpio_num_t resetPin, int mosi, int miso, int sclk, int cs, int freq = freq_);

    // 描画色の設定
    // r, g, b : 各色の値
    void setFigColor(uint8_t r, uint8_t g, uint8_t b);

    // 描画色の設定
    // c : 色コード
    void setFigColor(uint16_t c);

    // 四角形の描画
    // x, y : 左下？座標
    // width, height : 幅・高さ
    void drawRectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height);

    // 文字の描画位置
    // x, y : 左下？座標
    void setCharPlace(uint8_t x, uint8_t y);

    // 文字色の設定
    // r, g, b : 各色の値
    void setCharColor(uint8_t r, uint8_t g, uint8_t b);

    // 文字色の設定
    // c : 色コード
    void setCharColor(uint16_t c);

    // 文字背景色の設定
    // r, g, b : 各色の値
    void setCharColorBG(uint8_t r, uint8_t g, uint8_t b);

    // 文字背景色の設定
    // c : 色コード
    void setCharColorBG(uint16_t c);

    // 1文字描画
    // ch : 描画文字コード
    void print(char ch);

    // 文字列描画
    // str : 描画文字列
    void print(const std::string &str);

    // ATM0130へのデータ転送
    void start();

    // ATM0130へのデータ転送終了
    void end();

    // LockGuardに引数を合わせる
    void lock()
    {
      start();
    }

    void unlock()
    {
      end();
    }

  private:
    // コマンド書き込み
    // data : コマンド番号
    void writeReg(uint8_t data);

    // データ書き込み
    // data : データ
    void writeData(uint8_t data);

    // データ書き込み(1～4バイト)
    // len : 書き込みバイト数
    // d1~D4 : データ
    void writeData(size_t len, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4);

    // データ書き込み
    // len : 書き込みバイト数
    // buffer : データ
    void writeData(size_t len, uint8_t *buffer);

    // LCDのリセットコード
    void resetLCD(void);

    // 描画エリアの設定
    // x, y : 左下？座標
    // width, height : 幅、高さ
    void setWindow(uint8_t x, uint8_t y, uint8_t width, uint8_t height);

    // 1ドットの色の送信
    // color : 色コード
    void putPixel(uint16_t color);

    // setWindowの終了命令
    void endWindow();

    // SPI転送の結果受け取り
    void queResults(int pos = 0);

    // 描画文字を設定
    // c : 文字コード
    void setCharQueue(uint8_t c);

    // 文字ビットデータをATM0130に送信
    void writeCharQueue();

    // RGBを転送用のデータに変換
    // red, green, blue : 各色の値
    // return : 転送用の色情報
    static uint16_t convRGB(uint8_t red, uint8_t green, uint8_t blue);

    // 文字ビットパターンの一時的格納領域
    // 1文字分を格納
    uint8_t char_queue[5];

    // 文字のビットパターン
    // ASCII文字を格納？
    // 定義はcpp内
    static const uint8_t chars[];
  };

} // namespace Take4
