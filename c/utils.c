#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

long safe_strtol(const char *str, int *isError, int base){

	char *p;
    long ret;
    *isError = 0;

    errno = 0;
    ret = strtol(str, &p, base);

    if (p == str)
        *isError = 1;
    else if ((ret == LONG_MIN || ret == LONG_MAX) && errno == ERANGE)
        *isError = 1;
    else
        return ret;

    return 0;
}
