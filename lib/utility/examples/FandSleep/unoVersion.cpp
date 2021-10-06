#include <FunctionAndSleep.hpp>

Take4::FunctionAndSleep<5> func;
bool started = false;

void setup()
{
    Serial.begin(115200);
    // push_backで処理内容とその後の待機時間(ms)を設定する
    func.push_back([] {
        Serial.print("hoge1 ");
        Serial.println(func.millisec());
    },
                   1000);
    func.push_back([] {
        Serial.print("hoge2 ");
        Serial.println(func.millisec());
    },
                   100);
    func.push_back([] {
        Serial.print("hoge3 ");
        Serial.println(func.millisec());
    },
                   1200);
    func.push_back([] {
        Serial.print("hoge4 ");
        Serial.println(func.millisec());
    },
                   2500);
    func.push_back([] {
        Serial.print("hoge5 ");
        Serial.println(func.millisec());
    },
                   700);
    func.push_back([] {
        Serial.print("hoge6 ");
        Serial.println(func.millisec());
    },
                   300);
}

void loop()
{
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