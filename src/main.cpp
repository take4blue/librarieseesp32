#include <stdio.h>
#include <stdlib.h>
#include "Readline.h"

static char prompt[2][BUFSIZ];

extern "C"
void app_main()
{
    Take4::Readline reader;

    for (size_t counter = 0;; ++counter) {
        sprintf(prompt[counter % 2], "%d > ", counter);
        for (size_t i = 0; i < 100000; ++i) {
            if (reader.read(prompt[counter % 2])) {
                auto str = reader.get();
                char *error;
                auto val = strtol(str, &error, 10);
                printf("val = %ld\n", val);
            }
        }
    }
}