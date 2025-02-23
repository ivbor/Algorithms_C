#ifndef Mystrdup
#define Mystrdup
#include <stdlib.h>
#include <string.h>
#include "mystrcat.c"

char *mystrdup(const char *src, char *destination) {
    char *mut_src = strdup(src);
    destination = mystrcat(destination, mut_src);
    free(mut_src);
    return destination;
}
#endif
