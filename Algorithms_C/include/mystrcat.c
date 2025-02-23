#ifndef Mystrcat
#define Mystrcat

char *mystrcat(char *dest, char *src) {
    while (*dest)
        dest++;
    while ((*dest++ = *src++))
        ;
    return --dest;
}

char *const_mystrcat(char *dest, const char *src) {
    while (*dest)
        dest++;
    while ((*dest++ = *src++))
        ;
    return --dest;
}
#endif
