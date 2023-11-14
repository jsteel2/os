#include "virt.h"
#include "kmem.h"
#include "page.h"

// https://github.com/blanham/liballoc

BoundaryTag *free_pages[MAXEXP] = {0};
int complete_pages[MAXEXP] = {0};

int getexp(size_t size)
{
    if (size < (1 << MINEXP)) return -1;

    int shift = MINEXP;
    while (shift < MAXEXP && ((size_t)1 << shift) <= size) shift++;
    return shift - 1;
}

BoundaryTag *alloc_tag(size_t size)
{
    size_t usage = size + sizeof(BoundaryTag);
    size_t pages = usage / PAGE_SIZE;
    if ((usage % PAGE_SIZE) != 0) pages++;
    if (pages < MIN_PAGE_COUNT) pages = MIN_PAGE_COUNT;

    BoundaryTag *tag = (BoundaryTag *)virt_pages_alloc(&kernel_table, pages, ENTRY_R | ENTRY_W);
    tag->real_size = pages * PAGE_SIZE;
    tag->size = size;
    tag->index = -1;
    tag->next = NULL;
    tag->prev = NULL;
    tag->split_right = NULL;
    tag->split_left = NULL;

    return tag;
}

void remove_tag(BoundaryTag *tag)
{
    if (tag->index >= 0 && free_pages[tag->index] == tag) free_pages[tag->index] = tag->next;
    if (tag->prev) tag->prev->next = tag->next;
    if (tag->next) tag->next->prev = tag->prev;

    tag->next = NULL;
    tag->prev = NULL;
    tag->index = -1;
}

void insert_tag(BoundaryTag *tag, int index)
{
    if (index < 0)
    {
        index = getexp(tag->real_size - sizeof(BoundaryTag));
        if (index < MINEXP) index = MINEXP;
    }

    tag->index = index;
    if (free_pages[index])
    {
        free_pages[index]->prev = tag;
        tag->next = free_pages[index];
    }

    free_pages[index] = tag;
}

BoundaryTag *split_tag(BoundaryTag *tag)
{
    size_t remainder = tag->real_size - sizeof(BoundaryTag) - tag->size;

    BoundaryTag *new = tag + sizeof(BoundaryTag) + tag->size;
    new->real_size = remainder;
    new->next = NULL;
    new->prev = NULL;
    new->split_left = tag;
    new->split_right = tag->split_right;
    if (new->split_right) new->split_right->split_left = new;
    tag->split_right = new;
    tag->real_size -= new->real_size;

    insert_tag(new, -1);
    return new;
}

BoundaryTag *melt_left(BoundaryTag *tag)
{
    BoundaryTag *left = tag->split_left;

    left->real_size += tag->real_size;
    left->split_right = tag->split_right;

    if (tag->split_right) tag->split_right->split_left = left;

    return left;
}

BoundaryTag *absorb_right(BoundaryTag *tag)
{
    BoundaryTag *right = tag->split_right;

    remove_tag(right);
    tag->real_size += right->real_size;
    tag->split_right = right->split_right;

    if (right->split_right) right->split_right->split_left = tag;

    return tag;
}

void *malloc(size_t size)
{
    size_t index = getexp(size);
    if (index < MINEXP) index = MINEXP;

    BoundaryTag *tag = free_pages[index];
    while (tag)
    {
        if (tag->real_size - sizeof(BoundaryTag) >= size + sizeof(BoundaryTag)) break;
        tag = tag->next;
    }

    if (!tag)
    {
        tag = alloc_tag(size);
        index = getexp(tag->real_size - sizeof(BoundaryTag));
    }
    else
    {
        remove_tag(tag);
        if (!tag->split_left && !tag->split_right) complete_pages[index] -= 1;
    }

    tag->size = size;

    size_t remainder = tag->real_size - size - sizeof(BoundaryTag) * 2;
    if ((int)remainder > 0)
    {
        int child_index = getexp(remainder);
        if (child_index >= 0)
        {
            split_tag(tag);
        }
    }

    return tag + sizeof(BoundaryTag);
}

void free(void *p)
{
    if (!p) return;

    BoundaryTag *tag = p - sizeof(BoundaryTag);

    while (tag->split_left && tag->split_left->index >= 0)
    {
        tag = melt_left(tag);
        remove_tag(tag);
    }

    while (tag->split_right && tag->split_right->index >= 0)
    {
        tag = absorb_right(tag);
    }

    int index = getexp(tag->real_size - sizeof(BoundaryTag));
    if (index < MINEXP) index = MINEXP;

    if (!tag->split_left && !tag->split_right)
    {
        if (complete_pages[index] == MAXCOMPLETE)
        {
            size_t pages = tag->real_size / PAGE_SIZE;
            if ((tag->real_size % PAGE_SIZE) != 0) pages++;
            if (pages < MIN_PAGE_COUNT) pages = MIN_PAGE_COUNT;
            virt_pages_free(&kernel_table, (uint8_t *)tag, pages);
        }
        complete_pages[index]++;
    }

    insert_tag(tag, index);
}

void *realloc(void *p, size_t size)
{
    if (size == 0)
    {
        free(p);
        return NULL;
    }

    if (!p) return malloc(size);

    BoundaryTag *tag = p - sizeof(BoundaryTag);

    if (size < tag->real_size)
    {
        tag->size = size;
        return p;
    }

    // moving memory around like this is unneeded since we have virtual memory
    // but im not sure how that would work with this allocator
    // but this is only used in the kernel anyway so..
    void *ret = malloc(size);
    for (size_t i = 0; i < tag->size; i++) ((uint8_t *)ret)[i] = ((uint8_t *)p)[i];
    free(p);

    return ret;
}
