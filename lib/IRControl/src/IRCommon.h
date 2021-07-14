#pragma once
#include <stdint.h>
#if defined(ARDUINO_ARCH_AVR)
typedef uint8_t PortType_t;
typedef uint8_t PortNo_t;
#elif defined(ESP_PLATFORM)
typedef uint32_t PortType_t;
typedef uint32_t PortNo_t;
#endif

namespace Take4 {
// AHEAフォーマットで受け取れる最大DATA個数(8ビットで表されるD1からの数)
// ビエラのリモコンコードを調べたら3バイトしか使っていなかったので、+1の4で定義
const uint16_t MAX_AEHADATA = 50;

enum IRFormatType {
    NOFormat = 0, // 解析結果が得られていない状態
    NECFormat = 1, // NECフォーマット
    AEHAFormat = 2, // AEHA(家製協)フォーマット
    SONYFormat = 3, // SONYフォーマット

    NECFormatRepeat = 5, // NECのリピート情報
    AEHAFormatRepeat = 6, // AEHA(家庭協)のリピート情報
    SONYFormatRepeat = 7, // SONYのリピート情報
};

struct NECData {
    IRFormatType formatType;
    uint16_t customerCode;
    uint8_t data;
};

struct AEHAData {
    IRFormatType formatType;
    uint16_t customerCode;
    uint8_t parityData0;
    uint8_t nData;
    uint8_t data[MAX_AEHADATA];
};

struct SONYData {
    IRFormatType formatType;
    uint8_t data;
    uint16_t address;
	uint8_t nAddress;
};

union IRData {
    IRFormatType formatType;
    NECData nec;
    AEHAData aeha;
    SONYData sony;

    // コードの一致判断用
    bool isEqual(const IRData& value) const;
};
}
