#include "kmem.h"

usize strlen(const char *s)
{
    usize i;
    for (i = 0; s[i]; i++);
    return i;
}

int memcmp(const void *s1, const void *s2, usize n)
{
    const unsigned char *us1 = s1;
    const unsigned char *us2 = s2;
    for (usize i = 0; i < n; i++)
    {
        if (us1[i] != us2[i]) return (int)(us1[i] - us2[i]);
    }
    return 0;
}

void *memcpy(void *dest, const void *src, usize n)
{
    unsigned char *d = dest;
    const unsigned char *s = src;
    while (n--) *d++ = *s++;
    return dest;
}

void *memmove(void *dest, const void *src, usize n)
{
    unsigned char *d = dest;
    const unsigned char *s = src;

    if (d == s) return dest;

    if (d < s) return memcpy(dest, src, n);

    for (usize i = n; i > 0; i--) d[i - 1] = s[i - 1];

    return dest;
}

void *memchr(const void *s, int c, usize n)
{
    const unsigned char *p = s;
    while (n--)
    {
        if (*p == c) return (void *)p;
        p++;
    }

    return NULL;
}

char *strrchr(const char *s, int c)
{
    char *r = NULL;

    for (; *s; s++)
    {
        if (*s == c) r = (char *)s;
    }

    return r;
}

usize strnlen(const char *s, usize maxlen)
{
    const char *p = memchr(s, 0, maxlen);
    return p ? (usize)(p - s) : maxlen;
}
