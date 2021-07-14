#pragma once
#include <stdint.h>

const uint32_t T1NECFormat = 562;    // NECフォーマットでの1Tの時間(μ秒)
const uint32_t T1AEHAFormat = 425;   // AEHAフォーマットでの1Tの時間(μ秒)
const uint32_t T1SONYFormat = 600;   // SONYフォーマットでの1Tの時間(μ秒)

const uint32_t TFrameLeadLNEC =  T1NECFormat * 15.5;   // NECフォーマットでのFrame開始時のLeader Low認識時間(本来は16T)
const uint32_t TFrameLeadLAHEA = T1AEHAFormat * 7.5;   // AHEAフォーマットでのFrame開始時のLeader Low認識時間(本来は8T)
const uint32_t TFrameLeadLSONY = T1SONYFormat * 3.5;   // SONYフォーマットでのFrame開始時のLeader Low認識時間(本来は4T)

const uint32_t TFrameLeadHNEC =  T1NECFormat * 7.5;    // NECフォーマットでのFrame開始時のLeader High認識時間(本来は8T)
const uint32_t TFrameLeadHAHEA = T1AEHAFormat * 3.5;   // AHEAフォーマットでのFrame開始時のLeader High認識時間(本来は4T)
const uint32_t TFrameRepeatHNEC =  T1NECFormat * 3.5;  // NECフォーマットでのFrame開始時のRepeat High認識時間(本来は4T)
const uint32_t TFrameRepeatHAHEA = T1AEHAFormat * 7.5; // AHEAフォーマットでのFrame開始時のRepeat High認識時間(本来は8T)