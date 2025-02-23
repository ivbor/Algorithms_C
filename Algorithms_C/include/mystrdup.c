#ifndef Mystrdup
#define Mystrdup
#include <string.h>
#include <stdlib.h>
#include "mystrcat.c"

char *mystrdup(const char* src, char* destination) {
    char* mut_src = strdup(src);
    destination = mystrcat(destination, mut_src);
    free(mut_src);
    return destination;
}
#endif
