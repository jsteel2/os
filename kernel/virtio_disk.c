#include "virtio_disk.h"
#include "kprint.h"
#include "pmm.h"
#include "kmem.h"
#include "virtio.h"

VirtioDisk *virtio_disks = NULL;
usize virtio_disk_amt = 0;

void virtio_init_disk(u64 addr)
{
    *VR(addr, VIRTIO_MMIO_STATUS) = 0;
    u32 status = VIRTIO_STATUS_ACKNOWLEDGE;
    *VR(addr, VIRTIO_MMIO_STATUS) = status;
    status |= VIRTIO_STATUS_DRIVER_OK;
    *VR(addr, VIRTIO_MMIO_STATUS) = status;

    u32 host_features = *VR(addr, VIRTIO_MMIO_HOST_FEATURES);
    *VR(addr, VIRTIO_MMIO_GUEST_FEATURES) = host_features;

    status |= VIRTIO_STATUS_FEATURES_OK;
    *VR(addr, VIRTIO_MMIO_STATUS) = status;

    u32 qnmax = *VR(addr, VIRTIO_MMIO_QUEUE_NUM_MAX);
    *VR(addr, VIRTIO_MMIO_QUEUE_NUM) = VIRTIO_RING_SIZE;
    if (VIRTIO_RING_SIZE > qnmax)
    {
        kprintf("VIRTIO_RING_SIZE too big for virtio disk at 0x%x", addr);
        return;
    }

    usize num_pages = (sizeof(VirtioDisk) + PAGE_SIZE - 1) / PAGE_SIZE;
    *VR(addr, VIRTIO_MMIO_QUEUE_SEL) = 0;

    VirtioDisk *disk = pmm_alloc_continuous(num_pages);
    disk->addr = addr;
    disk->idx = 0;
    u32 pfn = (usize)&disk->queue / PAGE_SIZE;
    *VR(addr, VIRTIO_MMIO_GUEST_PAGE_SIZE) = PAGE_SIZE;
    *VR(addr, VIRTIO_MMIO_QUEUE_PFN) = pfn;

    //virtio_disks = realloc(virtio_disks, ++virtio_disk_amt * sizeof(VirtioDisk *));
    //virtio_disks[virtio_disk_amt - 1] = disk;

    status |= VIRTIO_STATUS_DRIVER_OK;
    *VR(addr, VIRTIO_MMIO_STATUS) = status;

    u8 *wbuf = pmm_alloc_continuous(1);
    memcpy(wbuf, "they call me the virtio disk driver", 36);
    virtio_disk_op(disk, 1, 0, wbuf);
}

u16 fill_next_descriptor(VirtioDisk *disk, VirtioDescriptor desc)
{
    disk->idx = (disk->idx + 1) % VIRTIO_RING_SIZE;
    if (desc.flags & VIRTIO_DESC_F_NEXT) desc.next = (disk->idx + 1) % VIRTIO_RING_SIZE;
    disk->queue.desc[disk->idx] = desc;
    return disk->idx;
}

// needs to be locked
void virtio_disk_op(VirtioDisk *disk, usize sectors, u64 sector_offset, u8 *wbuf)
{
    VirtioDiskRequest *req;
    if (wbuf) req = pmm_alloc_continuous((sizeof(VirtioDiskRequest) + PAGE_SIZE - 1) / PAGE_SIZE);
    else req = pmm_alloc_continuous((sizeof(VirtioDiskRequest) + sectors * 512 + PAGE_SIZE - 1) / PAGE_SIZE);
    req->header.blktype = wbuf ? VIRTIO_DISK_T_OUT : VIRTIO_DISK_T_IN;
    req->header.sector = sector_offset;
    req->header.reserved = 0;
 
    usize head_idx = fill_next_descriptor(disk, (VirtioDescriptor){(u64)&req->header, sizeof(VirtioDiskHeader), VIRTIO_DESC_F_NEXT, 0});

    fill_next_descriptor(disk, (VirtioDescriptor){wbuf ? (u64)wbuf : (u64)&req->data, sectors * 512, VIRTIO_DESC_F_NEXT | (wbuf ? 0 : VIRTIO_DESC_F_WRITE), 0});
    req->status = 111;
    fill_next_descriptor(disk, (VirtioDescriptor){(u64)&req->status, sizeof(u8), VIRTIO_DESC_F_WRITE, 0});
    disk->queue.avail.ring[disk->queue.avail.idx] = head_idx;
    disk->queue.avail.idx = (disk->queue.avail.idx + 1) % VIRTIO_RING_SIZE;
    *VR(disk->addr, VIRTIO_MMIO_QUEUE_NOTIFY) = 0;
}
