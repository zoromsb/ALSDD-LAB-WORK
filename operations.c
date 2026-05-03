#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "operations.h"

void to_lowercase(char *s) {
    for (int i = 0; s[i]; i++)
        s[i] = tolower(s[i]);
}
