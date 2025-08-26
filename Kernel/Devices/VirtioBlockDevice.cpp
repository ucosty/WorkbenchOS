// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include "VirtioBlockDevice.h"
#include "PhysicalAddress.h"
#include <UnbufferedConsole.h>
#include <Platform/x86_64/PortIO.h>

#include "Memory/MemoryManager.h"

struct BarInfo {
    bool is_io;
    bool is_mem64;
    uint16_t io_base; // if is_io
    uint64_t mmio_phys; // if !is_io
};

static BarInfo decode_bar(uint32_t bar_lo, uint32_t bar_hi_if_64 = 0) {
    BarInfo r{};
    if (bar_lo & 0x1) {
        // I/O BAR
        r.is_io = true;
        r.io_base = static_cast<uint16_t>(bar_lo & ~0x3u);
    } else {
        // MMIO BAR
        r.is_io = false;
        bool is64 = ((bar_lo & 0x6) == 0x4); // bits 2:1 == 10b
        r.is_mem64 = is64;
        if (is64) {
            r.mmio_phys = (static_cast<uint64_t>(bar_hi_if_64) << 32) | static_cast<uint64_t>(bar_lo & ~0xFu);
        } else {
            r.mmio_phys = static_cast<uint64_t>(bar_lo & ~0xFu);
        }
    }
    return r;
}

void hexdump(const u8 *buffer, const size_t size);

Std::Result<VirtioBlockDevice> VirtioBlockDevice::detect_and_init(PCI *pci) {
    auto pci_device_or_empty = pci->find_device(0x1af4, 0x1001);
    if (!pci_device_or_empty.is_present()) {
        Std::Error::from_code(1);
    }

    auto device = VirtioBlockDevice{};

    const auto pci_device = pci_device_or_empty.get();

    auto bar0 = decode_bar(pci_device->bar0());

    if (bar0.is_io) {
        println("virtio-blk: device is io mapped");

        if (!device.init(bar0.io_base)) {
            println("virtio-blk: init failed");
            return Std::Error::from_code(1);
        }
        println("virtio-blk: capacity = {} sectors ({} MiB)",
                device.capacity_sectors(), (device.capacity_sectors() / 2048));

        // char buffer[512] = {};
        // const auto result = device.read(0, 512, &buffer);
        // TRY(device.rw(/*write=*/false, 0, /*src=*/nullptr, /*dst=*/buffer, 512));

        // if (result.is_error()) {
            // println("virtio-blk: read failed with code {}", result.get_error().get_code());
        // } else {
            // hexdump(reinterpret_cast<const u8 *>(buffer), 100);
        // }

        return device;
    }

    if (bar0.is_mem64) {
        println("virtio-blk: device is memory-mapped");
    }

    return Std::Error::from_code(1);
}

bool VirtioBlockDevice::init(u16 io_base) {
    m_io_base = io_base;

    // 0) Reset status
    wr8(VIRTIO_PCI_STATUS, 0);

    // 1) ACK + DRIVER
    wr8(VIRTIO_PCI_STATUS, reg8(VIRTIO_PCI_STATUS) | S_ACK);
    wr8(VIRTIO_PCI_STATUS, reg8(VIRTIO_PCI_STATUS) | S_DRIVER);

    // 2) Feature negotiation: take none for now
    (void) reg32(VIRTIO_PCI_HOST_FEATURES);
    wr32(VIRTIO_PCI_GUEST_FEATURES, 0);
    wr8(VIRTIO_PCI_STATUS, reg8(VIRTIO_PCI_STATUS) | S_FEATURES_OK);
    if ((reg8(VIRTIO_PCI_STATUS) & S_FEATURES_OK) == 0) {
        wr8(VIRTIO_PCI_STATUS, S_FAILED);
        println("VirtioBlockDevice::init: failed VIRTIO_PCI_STATUS");
        return false;
    }

    // 3) Queue setup (queue 0)
    if (!setup_queue0(/*preferred_num=*/0)) {
        wr8(VIRTIO_PCI_STATUS, S_FAILED);
        println("VirtioBlockDevice::init: failed to init queue");
        return false;
    }

    // 4) Read capacity (u64 sectors) from device config @ +0x14
    uint16_t cfg = VIRTIO_PCI_DEVICE_CONFIG;
    uint32_t cap_lo = reg32(cfg + 0);
    uint32_t cap_hi = reg32(cfg + 4);
    m_capacity_sectors = (uint64_t(cap_hi) << 32) | cap_lo;

    // 5) DRIVER_OK
    wr8(VIRTIO_PCI_STATUS, uint8_t(reg8(VIRTIO_PCI_STATUS) | S_DRIVER_OK));

    return true;
}

u8 VirtioBlockDevice::reg8(const u16 offset) const { return PortIO::in8(m_io_base + offset); }

u16 VirtioBlockDevice::reg16(const u16 offset) const { return PortIO::in16(m_io_base + offset); }

u32 VirtioBlockDevice::reg32(const u16 offset) const { return PortIO::in32(m_io_base + offset); }

void VirtioBlockDevice::wr8(const u16 offset, const u8 value) const { PortIO::out8(m_io_base + offset, value); }

void VirtioBlockDevice::wr16(const u16 offset, const u16 value) const { PortIO::out16(m_io_base + offset, value); }

void VirtioBlockDevice::wr32(const u16 offset, const u32 value) const { PortIO::out32(m_io_base + offset, value); }

static size_t vring_size_bytes(uint16_t num) {
    size_t sz = num * sizeof(VDesc);
    sz += sizeof(VAvail) + num * sizeof(uint16_t);
    sz = (sz + VRING_ALIGN - 1) & ~(size_t) (VRING_ALIGN - 1);
    sz += sizeof(VUsed) + num * sizeof(VUsedEl);
    return sz;
}

uint16_t VirtioBlockDevice::pop_free() {
    if (m_q.free_count == 0) return 0xFFFF;
    return m_q.free[--m_q.free_count];
}

void VirtioBlockDevice::push_free(uint16_t idx) {
    m_q.free[m_q.free_count++] = idx;
}


bool VirtioBlockDevice::setup_queue0(uint16_t preferred_num) {
    // Select queue 0
    wr16(VIRTIO_PCI_QUEUE_SEL, 0);

    uint16_t qmax = reg16(VIRTIO_PCI_QUEUE_NUM);
    if (qmax == 0) {
        println("VirtioBlockDevice::setup_queue0: no queues");
        return false;
    };

    m_queue_num = (preferred_num && preferred_num <= qmax) ? preferred_num : qmax;

    // Compute ring size and allocate contiguous pages (<4GiB not strictly required for data,
    // but legacy QUEUE_PFN is 32-bit so the ring base must be <4GiB).
    m_ring_bytes = vring_size_bytes(m_queue_num);
    size_t pages = (m_ring_bytes + 4095) / 4096;

    auto &mm = Kernel::MemoryManager::get_instance();
    auto res = mm.allocate_physical_pages(pages);
    if (res.is_error()) {
        println("VirtioBlockDevice::setup_queue0: failed to allocate physical pages");
        return false;
    };

    m_ring_phys = res.get(); // adjust to your Result<T> API
    auto ring_va = m_ring_phys.as_virtual_address();
    m_ring_va = reinterpret_cast<void *>(ring_va.as_address());

    // Layout
    uint8_t *p = static_cast<uint8_t *>(m_ring_va);
    m_q.num = m_queue_num;
    m_q.desc = reinterpret_cast<VDesc *>(p);
    p += m_queue_num * sizeof(VDesc);
    m_q.avail = reinterpret_cast<VAvail *>(p);
    p += sizeof(VAvail) + m_queue_num * sizeof(uint16_t);
    // align up for used
    uintptr_t up = reinterpret_cast<uintptr_t>(p);
    up = (up + VRING_ALIGN - 1) & ~(uintptr_t) (VRING_ALIGN - 1);
    m_q.used = reinterpret_cast<VUsed *>(up);

    // init used shadow
    m_q.used_idx_shadow = m_q.used->idx = 0;
    m_q.avail->idx = 0;

    // init free list
    m_q.free_count = m_queue_num;
    // place the free list just after used (scratch; or keep a small array elsewhere)
    static_assert(sizeof(uint16_t) == 2, "assume 16-bit");
    // For clarity, allocate separate tiny array from normal allocator if you prefer:
    m_q.free = new uint16_t[m_queue_num];
    for (uint16_t i = 0; i < m_queue_num; ++i) m_q.free[i] = m_queue_num - 1 - i;

    // Hand PFN to device (legacy)
    uint64_t phys = m_ring_phys.as_virtual_address().as_address(); // physical wanted; see note below
    // NOTE: you said: PhysicalAddress::as_virtual_address() returns VA mapped to the same PA.
    // We need the PHYSICAL address here:
    phys = m_ring_phys.as_address(); // <-- use the PhysicalAddress method that returns PA
    uint32_t pfn = (uint32_t) (phys >> 12);
    wr32(VIRTIO_PCI_QUEUE_PFN, pfn);

    return true;
}

Std::Result<void> VirtioBlockDevice::rw(bool write, uint64_t lba,
                                        const void *src_or_null, void *dst_or_null, uint32_t bytes) {
    const auto memory_manager = &Kernel::MemoryManager::get_instance();

    // Sanity
    if (bytes == 0 || (bytes % 512) != 0) return Std::Error::from_code(1);

    // 3 descriptors: header, data, status
    uint16_t d0 = pop_free();
    uint16_t d1 = pop_free();
    uint16_t d2 = pop_free();
    if (d0 == 0xFFFF || d1 == 0xFFFF || d2 == 0xFFFF) Std::Error::from_code(2);

    // Staging small objects (you may want per-request storage later)
    alignas(16) BlkReq hdr{};
    hdr.type = write ? 1u : 0u;
    hdr.reserved = 0;
    hdr.sector = lba;

    volatile uint8_t status = 0xFF;

    // Translate to phys addrs
    auto hdr_pa = TRY(memory_manager->kernel_virtual_to_physical_address(reinterpret_cast<u64>(&hdr)));
    auto st_pa = TRY(memory_manager->kernel_virtual_to_physical_address(reinterpret_cast<u64>(&status)));
    auto buf_pa = TRY(
        memory_manager->kernel_virtual_to_physical_address(write ? reinterpret_cast<u64>(src_or_null): reinterpret_cast<
            u64>(dst_or_null)));

    auto buffer = write ? reinterpret_cast<u64>(src_or_null): reinterpret_cast<u64>(dst_or_null);

    // 1) header (device reads)
    m_q.desc[d0].addr = hdr_pa.as_address();
    m_q.desc[d0].len = sizeof(hdr);
    m_q.desc[d0].flags = VRING_DESC_F_NEXT;
    m_q.desc[d0].next = d1;

    // 2) data
    m_q.desc[d1].addr = buf_pa.as_address();
    m_q.desc[d1].len = bytes;
    m_q.desc[d1].flags = VRING_DESC_F_NEXT | (write ? 0 : VRING_DESC_F_WRITE);
    m_q.desc[d1].next = d2;

    // 3) status (device writes 1 byte)
    m_q.desc[d2].addr = st_pa.as_address();
    m_q.desc[d2].len = 1;
    m_q.desc[d2].flags = VRING_DESC_F_WRITE;
    m_q.desc[d2].next = 0;

    // sanity check again
    // println("Header: virt = {:#x}, phys = {:#x}", (void*)&hdr, hdr_pa.as_address());
    // println("Buffer: virt = {:#x}, phys = {:#x}", buffer, buf_pa.as_address());
    // println("Status: virt = {:#x}, phys = {:#x}", (void*)&status, st_pa.as_address());

    // Publish to avail
    uint16_t *aring = vring_avail_ring(m_q.avail);
    uint16_t aidx = m_q.avail->idx;
    aring[aidx % m_q.num] = d0;
    asm volatile("" ::: "memory"); // publish barrier
    m_q.avail->idx = aidx + 1;

    // Notify device (queue 0)
    wr16(VIRTIO_PCI_QUEUE_NOTIFY, 0);

    // Poll for completion
    while (m_q.used->idx == m_q.used_idx_shadow) {
        // busy-wait; you can add cpu_relax()
    }

    // One completion
    VUsedEl e = vring_used_ring(m_q.used)[m_q.used_idx_shadow % m_q.num];
    m_q.used_idx_shadow++;

    // Reclaim descriptors (the chain head id should be d0)
    (void) e;
    push_free(d0);
    push_free(d1);
    push_free(d2);

    if (status == 0) {
        return {};
    }
    return Std::Error::from_code(status);
}

Std::Result<void> VirtioBlockDevice::read(const size_t offset, const size_t size, void *buffer) {
    return rw(/*write=*/false, offset, /*src=*/nullptr, /*dst=*/buffer, size);
}

Std::Result<void> VirtioBlockDevice::write(const size_t offset, const size_t size, void *buffer) {
    return rw(/*write=*/true, offset, /*src=*/buffer, /*dst=*/nullptr, size);
}
