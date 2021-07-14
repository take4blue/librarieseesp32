#include <IRin.h>

using namespace Take4;

IRFormatType IRin::formatType() const
{
    return data_.formatType;
}

const IRData& IRin::data() const
{
    return data_;
}

bool IRin::isRepeat() const
{
    return (data_.formatType & 0x4) != 0 ? true : false;
}

bool IRData::isEqual(const IRData& value) const
{
    if ((formatType & 0x3) == (value.formatType & 0x3)) {
        switch ((formatType & 0x3)) {
        case NECFormat:
            if (nec.customerCode == value.nec.formatType && nec.data && value.nec.data) {
                return true;
            } else {
                return false;
            }
            break;
        case AEHAFormat:
            if (aeha.customerCode == value.aeha.customerCode && aeha.parityData0 && value.aeha.parityData0 && aeha.nData == value.aeha.nData) {
                for (int i = 0; i < aeha.nData; i++) {
                    if (aeha.data[i] != value.aeha.data[i]) {
                        return false;
                    }
                }
                return true;
            }
            return false;
        case SONYFormat:
            if (sony.data == value.sony.data && sony.address == value.sony.address) {
                return true;
            } else {
                return false;
            }
            break;
        }
        return false;
    } else {
        return false;
    }
}