#if defined(IDF_VER)
#include <stdio.h>
#include <IRin.h>
const PortNo_t IRSEND_PIN = 3; // IRの受信ピン

Take4::IRin ii;

void setup()
{
    ii.begin(IRSEND_PIN, 0);
    printf("Start\n");
}

void loop()
{
    if (ii.parse()) {
        switch (ii.formatType()) {
        case Take4::NECFormat:
            printf("NEC %x %x\n", ii.data().nec.customerCode, ii.data().nec.data);
            break;
        case Take4::AEHAFormat:
            printf("AEHA %x %x ", ii.data().aeha.customerCode, ii.data().aeha.parityData0);
            for (unsigned int i = 0; i < ii.data().aeha.nData; i++) {
                printf("%x ", ii.data().aeha.data[i]);
            }
            printf("\n");
            break;
        case Take4::SONYFormat:
            printf("SONY %x %x\n", ii.data().sony.data, ii.data().sony.address);
            break;
        case Take4::NOFormat:
            printf("OFF\n");
            break;
        default:
            break;
        }
        if (ii.isRepeat()) {
            printf("Repeat\n");
        }
    }
}

extern "C" void app_main()
{
    setup();
    for (;;)
    {
        loop();
    }
}
#endif