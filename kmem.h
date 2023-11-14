#ifndef _KMEM_H
#define _KMEM_H

#include <stddef.h>

#define MAXEXP 32
#define MINEXP 8
#define MIN_PAGE_COUNT 16
#define MAXCOMPLETE 5

struct _boundary_tag
{
    size_t size;
    size_t real_size;
    int index;
    struct _boundary_tag *split_left;
    struct _boundary_tag *split_right;
    struct _boundary_tag *next;
    struct _boundary_tag *prev;
};

typedef struct _boundary_tag BoundaryTag;

void *malloc(size_t size);
void free(void *p);
void *realloc(void *p, size_t size);

#endif
