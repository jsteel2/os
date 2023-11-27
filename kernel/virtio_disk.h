#ifndef  _VIRTIO_DISK
#define _VIRTIO_DISK

#include "stdint.h"
#include "virtio.h"

#define VIRTIO_DISK_T_IN 0
#define VIRTIO_DISK_T_OUT 1

typedef struct
{
    VirtioQueue queue;
    u64 addr;
    u16 idx;
} VirtioDisk;

typedef struct __attribute((packed))__
{
    u32 blktype;
    u32 reserved;
    u64 sector;
} VirtioDiskHeader;

typedef struct
{
    VirtioDiskHeader header;
    u8 status;
    u16 head;
    u8 data[];
} VirtioDiskRequest;

void virtio_init_disk(u64 addr);
void virtio_disk_op(VirtioDisk *disk, usize sectors, u64 sector_offset, u8 *wbuf);

#endif
