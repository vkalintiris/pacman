#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <wchar.h>

#include "util-common.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size);

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    if (Size == 0)
        return 0;

    // Prepare a null terminated string
    char* cstring = malloc(Size+1);
    memcpy(cstring, Data, Size);
    cstring[Size] = 0;

    char** ptr = wordsplit(cstring);
    if (ptr)
        free(ptr);

    free(cstring);

    return 0;
}
