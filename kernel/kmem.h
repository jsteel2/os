#ifndef _KMEM_H
#define _KMEM_H

#include "stdint.h"

usize strlen(const char *s);
int memcmp(const void *s1, const void *s2, usize n);
void *memcpy(void *dest, const void *src, usize n);
void *memmove(void *dest, const void *src, usize n);
void *memchr(const void *s, int c, usize n);
char *strrchr(const char *s, int c);
usize strnlen(const char *s, usize maxlen);

#endif
