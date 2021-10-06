#include <FunctionAndSleep.hpp>

// <>内に登録可能最大数(uint8_t)を設定する
Take4::FunctionAndSleep<5> func;

extern "C" void app_main(void)
{
    // push_backで処理内容とその後の待機時間(ms)を設定する
    func.push_back([] {
        printf("hoge1 %llu\n", func.millisec());
    },
                   1000);
    func.push_back([] {
        printf("hoge2 %llu\n", func.millisec());
    },
                   100);
    func.push_back([] {
        printf("hoge3 %llu\n", func.millisec());
    },
                   1200);
    func.push_back([] {
        printf("hoge4 %llu\n", func.millisec());
    },
                   2500);
    func.push_back([] {
        printf("hoge5 %llu\n", func.millisec());
    },
                   700);
    func.push_back([] {
        printf("hoge6 %llu\n", func.millisec());
    },
                   300);

    bool started = false;
    for (;;) {
        if (!started) {
            func.start();
            started = true;
        }
        else {
            if (!func.doCheck()) {
                started = false;
            }
        }
    }
}