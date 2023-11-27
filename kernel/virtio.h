#ifndef _VIRTIO_H
#define _VIRTIO_H

#include "pmm.h"

#define VIRTIO_MMIO_MAGIC_VALUE 0x000
#define VIRTIO_MMIO_VERSION 0x004
#define VIRTIO_MMIO_DEVICE_ID 0x008
#define VIRTIO_MMIO_VENDOR_ID 0x00c
#define VIRTIO_MMIO_HOST_FEATURES 0x010
#define VIRTIO_MMIO_HOST_FEATURES_SEL 0x014
#define VIRTIO_MMIO_GUEST_FEATURES 0x020
#define VIRTIO_MMIO_GUEST_FEATURES_SEL 0x024
#define VIRTIO_MMIO_GUEST_PAGE_SIZE 0x028
#define VIRTIO_MMIO_QUEUE_SEL 0x030
#define VIRTIO_MMIO_QUEUE_NUM_MAX 0x034
#define VIRTIO_MMIO_QUEUE_NUM 0x038
#define VIRTIO_MMIO_QUEUE_ALIGN 0x03c
#define VIRTIO_MMIO_QUEUE_PFN 0x040
#define VIRTIO_MMIO_QUEUE_NOTIFY 0x050
#define VIRTIO_MMIO_INTERRUPT_STATUS 0x060
#define VIRTIO_MMIO_INTERRUPT_ACK 0x064
#define VIRTIO_MMIO_STATUS 0x070
#define VIRTIO_MMIO_CONFIG 0x100

#define VIRTIO_STATUS_ACKNOWLEDGE 1
#define VIRTIO_STATUS_DRIVER_OK 4
#define VIRTIO_STATUS_FEATURES_OK 8

#define VIRTIO_DESC_F_NEXT 1
#define VIRTIO_DESC_F_WRITE 2

#define VIRTIO_MAGIC_VALUE 0x74726976

#define VIRTIO_RING_SIZE 128

#define VR(addr, r) ((volatile u32 *)(addr + (r)))

typedef struct __attribute__((packed))
{
    u64 addr;
    u32 len;
    u16 flags;
    u16 next;
} VirtioDescriptor;

typedef struct __attribute__((packed))
{
    u16 flags;
    u16 idx;
    u16 ring[VIRTIO_RING_SIZE];
    u16 event;
} VirtioAvailable;

typedef struct __attribute__((packed))
{
    u32 id;
    u32 len;
} VirtioUsedElem;

typedef struct __attribute__((packed))
{
    u16 flags;
    u16 idx;
    VirtioUsedElem ring[VIRTIO_RING_SIZE];
    u16 event;
} VirtioUsed;

typedef struct __attribute__((packed))
{
    VirtioDescriptor desc[VIRTIO_RING_SIZE];
    VirtioAvailable avail;
    u8 padding[PAGE_SIZE - sizeof(VirtioDescriptor) * VIRTIO_RING_SIZE - sizeof(VirtioAvailable)];
    VirtioUsed used;
} VirtioQueue;

void virtio_init(void *fdt);

#endif
