#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <wchar.h>

#include "util.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size);

static int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    if (Size == 0)
        return 0;

    // Prepare a null terminated string
    char* cstring = malloc(Size+1);
    memcpy(cstring, Data, Size);
    cstring[Size] = 0;

    string_length(cstring);

    free(cstring);

    return 0;
}
