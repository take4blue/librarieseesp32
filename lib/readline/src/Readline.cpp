#include "Readline.h"
#include <iostream>
#include <string.h>

namespace Take4
{
    Readline::Readline()
    : pos_(0)
    , fixedLine_(false)
    , viewPrompt_(true)
    , prevPrompt_(nullptr)
    {
        buffer_[0] = 0;
    }

    Readline::~Readline()
    {
    }

    void Readline::deleteChar()
    {
        putchar(0x08);
        putchar(' ');
        putchar(0x08);
    }

    bool Readline::read(const char* prompt)
    {
        if (viewPrompt_) {
            if (prompt) {
                printf(prompt);
            }
            prevPrompt_ = prompt;
            viewPrompt_ = false;
        }
        else if (prompt != prevPrompt_) {
            // プロンプト文字列変更
            size_t length = pos_ + strlen(prevPrompt_);
            for (size_t i = 0; i < length; i++) {
                deleteChar();
            }
            if (prompt) {
                printf(prompt);
                for (size_t i = 0; i < pos_; i++) {
                    putchar(buffer_[i]);
                }
                prevPrompt_ = prompt;
            }
        }
        auto ch = getchar();
        if (ch != EOF) {
            if (fixedLine_) {
                pos_ = 0;
                fixedLine_ = false;
            }
            if (!iscntrl(ch)) {
                putchar(ch);
                buffer_[pos_++] = ch;
            }
            else if (pos_ > 0) {
                if (ch == 0x08) {
                    pos_--;
                    deleteChar();
                }
                else {
                    buffer_[pos_++] = 0;
                    fixedLine_ = true;
                    putchar('\n');
                    viewPrompt_ = true;
                    return true;
                }
            }
        }
        return false;
    }

    const char* Readline::get() const
    {
        if (fixedLine_) {
            return buffer_;
        }
        else {
            return nullptr;
        }
    }
} // namespace Take4
