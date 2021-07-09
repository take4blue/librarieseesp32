#pragma once
#include <stddef.h>

namespace Take4
{
    class Readline {
    private:
        static const size_t BufSize = 1024;

        char buffer_[BufSize];
        size_t pos_;
        bool fixedLine_;
        bool viewPrompt_;

        const char* prevPrompt_;

        void deleteChar();

    public:
        Readline();
        
        ~Readline();

        // 1行データの取得
        // prompt : コマンドプロンプト
        // return : trueの場合1行のデータが確定した場合
        bool read(const char* prompt);

        // 行情報の取得
        // 1行の情報が確定していない場合はnullptrになる
        const char* get() const;
    };
} // namespace Take4
