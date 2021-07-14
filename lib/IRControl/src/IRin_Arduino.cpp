#if defined(ARDUINO_ARCH_AVR)
#include <Arduino.h>
#include <IRin.h>
#include "IRTime.h"

using namespace Take4;

// NEC, AEHAフォーマットのデータ取り出しコード
// Lowの時に呼び出しHighの時間計測をして、0,1の判断をする
// 0は1T、1は3Tだが、識別条件は2.5Tにしている
uint32_t getDataH(uint16_t port, uint16_t nBit, uint16_t dt)
{
    uint32_t work = 0;

    for (uint16_t i = 0; i < nBit; i++) {
        uint32_t wTime = pulseIn(port, HIGH, 10000);
        if (wTime == 0) {
            // タイムアウト
            return ~0;
        } else if (wTime > dt * 2.5) {
            work |= 1 << i;
        }
    }
    return work;
}
// SONYフォーマットのデータ取り出しコード
// HIGHの時に呼び出しLowの時間計測をして、0,1の判断をする
// 0は1T、1は2Tだが、識別条件は1.5T以上としている
uint32_t getDataSony(uint16_t port, uint16_t nBit, uint16_t dt, uint16_t& readBit)
{
    uint32_t work = 0;
	readBit = nBit;

    for (uint16_t i = 0; i < nBit; i++) {
        uint32_t wTime = pulseIn(port, LOW, dt * 4);
        if (wTime == 0) {
			readBit = i;
            break;
        } else if (wTime > dt * 1.7) {
            work |= 1 << i;
        }
    }
    return work;
}

// 指定されたポートのOn/Offの変化時間を取り出す
// startTimeが、ひとつ前の変化の発生時間で、この関数の中で、ポートの初期値を取り出し、変化しているかの判断をする
uint32_t triggerChange(uint16_t port, uint32_t startTime, uint32_t timeOut)
{
    uint32_t work = PIND & _BV(port);
    uint32_t maxLoop = microsecondsToClockCycles(timeOut) / 16;
    uint32_t i;
    for (i = 0; i < maxLoop && (PIND & _BV(port)) == work; i++)
        ;
    uint32_t endTime = micros();
    if (i == maxLoop) {
        // タイムアウト
        return 0;
    }
    return endTime - startTime;
}

IRin::IRin()
    : pin_(0)
    , timeOut_(200000)
{
}

IRin::~IRin()
{    
}

void IRin::begin(PortNo_t pin)
{
    pin_ = pin;
    pinMode(pin_, INPUT);
}

bool IRin::parse()
{
    // 現在の状態取り出しからのフォーマット解析のため、Leaderフレーム時間を調査する
    uint32_t pTime = pulseIn(pin_, LOW, timeOut_);
    uint32_t lowStart = micros(); // Repeat検出用のHIGH時間計測開始

    // フォーマット検出
    IRFormatType wFormat = data_.formatType;
    if (pTime > TFrameLeadLNEC) {
        data_.formatType = NECFormat;
        timeOut_ = 109000 - pTime;
    } else if (pTime > TFrameLeadLAHEA) {
        data_.formatType = AEHAFormat;
        timeOut_ = 131000 - pTime;
    } else if (pTime > TFrameLeadLSONY) {
        data_.formatType = SONYFormat;
        timeOut_ = 80000 - pTime;
    } else {
        data_.formatType = NOFormat;
        timeOut_ = 200000;
    }

    // 個々のデータ読み込み
    if (data_.formatType == SONYFormat) {
		uint16_t nBit;
        uint32_t recvData = getDataSony(pin_, 20, T1SONYFormat, nBit);
        data_.sony.data = recvData & 0x7F;
        data_.sony.address = recvData >> 7;
		data_.sony.nAddress = nBit - 7;
        if (wFormat == SONYFormat || wFormat == SONYFormatRepeat) {
            data_.formatType = SONYFormatRepeat;
        } else {
            data_.formatType = SONYFormat;
        }
    } else if (data_.formatType == NECFormat || data_.formatType == AEHAFormat) {
        pTime = triggerChange(pin_, lowStart, timeOut_);
        if (data_.formatType == NECFormat) {
            if (pTime > TFrameLeadHNEC) {
                data_.formatType = NOFormat;
                uint32_t recvData = getDataH(pin_, 16, T1NECFormat);
                if (recvData != ~0) {
                    data_.nec.customerCode = recvData;
                    recvData = getDataH(pin_, 16, T1NECFormat);
                    if (recvData != ~0) {
                        data_.nec.data = recvData & 0xF;
                        if (wFormat == NECFormat || wFormat == NECFormatRepeat) {
                            data_.formatType = NECFormatRepeat;
                        } else {
                            data_.formatType = NECFormat;
                        }
                    }
                }
            } else if (pTime > TFrameRepeatHNEC) {
                data_.formatType = NECFormatRepeat;
            } else {
                data_.formatType = NOFormat;
            }
        } else {
            if (pTime > TFrameRepeatHAHEA) {
                data_.formatType = AEHAFormatRepeat;
            } else if (pTime > TFrameLeadHAHEA) {
                data_.formatType = NOFormat;
                uint32_t recvData = getDataH(pin_, 16, T1AEHAFormat);
                if (recvData != ~0) {
                    data_.aeha.customerCode = recvData;
                    recvData = getDataH(pin_, 8, T1AEHAFormat);
                    if (recvData != ~0) {
                        data_.aeha.parityData0 = recvData;
                        for (data_.aeha.nData = 0; data_.aeha.nData < MAX_AEHADATA; data_.aeha.nData++) {
                            recvData = getDataH(pin_, 8, T1AEHAFormat);
                            if (recvData == ~0) {
                                break;
                            }
                            data_.aeha.data[data_.aeha.nData] = recvData;
                        }
                        if (wFormat == AEHAFormat || wFormat == AEHAFormatRepeat) {
                            data_.formatType = AEHAFormatRepeat;
                        } else {
                            data_.formatType = AEHAFormat;
                        }
                    }
                }
            } else {
                data_.formatType = NOFormat;
            }
        }
    }
    return (wFormat == NOFormat && data_.formatType == NOFormat) ? false : true;
}
#endif