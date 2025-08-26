// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include <PhysicalAddress.h>
#include <Bus/PCI.h>
#include <Platform/x86_64/PortIO.h>

#include "BlockDevice.h"

// ---- VirtIO legacy (transitional) register offsets (I/O BAR) ----
enum : uint16_t {
    VIRTIO_PCI_HOST_FEATURES = 0x00, // 32
    VIRTIO_PCI_GUEST_FEATURES = 0x04, // 32
    VIRTIO_PCI_QUEUE_PFN = 0x08, // 32  (legacy: PFN = phys>>12)
    VIRTIO_PCI_QUEUE_NUM = 0x0C, // 16
    VIRTIO_PCI_QUEUE_SEL = 0x0E, // 16
    VIRTIO_PCI_QUEUE_NOTIFY = 0x10, // 16
    VIRTIO_PCI_STATUS = 0x12, // 8
    VIRTIO_PCI_ISR_STATUS = 0x13, // 8 (read-to-ack)
    VIRTIO_PCI_DEVICE_CONFIG = 0x14, // window (legacy layout)
};

// ---- Status bits ----
enum : uint8_t {
    S_ACK = 1,
    S_DRIVER = 2,
    S_DRIVER_OK = 4,
    S_FEATURES_OK = 8,
    S_FAILED = 0x80,
};

constexpr uint16_t VRING_DESC_F_NEXT = 1;
constexpr uint16_t VRING_DESC_F_WRITE = 2;
constexpr uint16_t VRING_ALIGN = 4096;

struct VDesc {
    uint64_t addr;
    uint32_t len;
    uint16_t flags, next;
};

struct VAvail {
    uint16_t flags, idx; /* followed by uint16_t ring[num]; then optional used_event */
};

struct VUsedEl {
    uint32_t id, len;
};

struct VUsed {
    uint16_t flags, idx; /* followed by VUsedEl ring[num]; then optional avail_event */
};

// ---- virtio-blk request header ----
struct __attribute__((packed)) BlkReq {
    uint32_t type;     // 0=READ, 1=WRITE, 4=FLUSH
    uint32_t reserved;
    uint64_t sector;   // LBA
};

// Helpers to get the trailing arrays
inline uint16_t* vring_avail_ring(VAvail* a) {
    return reinterpret_cast<uint16_t*>(a + 1);   // immediately after header
}
inline VUsedEl* vring_used_ring(VUsed* u) {
    return reinterpret_cast<VUsedEl*>(u + 1);  // immediately after header
}

class VirtioBlockDevice : public BlockDevice {
public:
    static Std::Result<VirtioBlockDevice> detect_and_init(PCI *pci);

    bool init(uint16_t iobase);

    Std::Result<void> read(size_t offset, size_t size, void *buffer);
    Std::Result<void> write(size_t offset, size_t size, void *buffer);

    [[nodiscard]] u64 capacity_sectors() const { return m_capacity_sectors; }

private:
    [[nodiscard]] inline u8 reg8(u16 offset) const;

    [[nodiscard]] inline u16 reg16(u16 offset) const;

    [[nodiscard]] inline u32 reg32(u16 offset) const;

    inline void wr8(u16 offset, u8 value) const;

    inline void wr16(u16 offset, u16 value) const;

    inline void wr32(u16 offset, u32 value) const;

    // ----- vring bookkeeping -----
    struct Vring {
        uint16_t num{};
        VDesc *desc{};
        VAvail *avail{};
        VUsed *used{};
        uint16_t used_idx_shadow{};
        // simple free list (stack of indices)
        uint16_t *free{};
        uint16_t free_count{};
    };

    bool setup_queue0(uint16_t preferred_num /*0=use device value*/);

    Std::Result<void> rw(bool write, uint64_t lba, const void *src_or_null, void *dst_or_null, uint32_t bytes);

    // Small helpers
    uint16_t pop_free();

    void push_free(uint16_t idx);

    uint16_t m_io_base{0};
    uint64_t m_capacity_sectors{0};

    // vring memory
    PhysicalAddress m_ring_phys{};
    void *m_ring_va{nullptr};
    size_t m_ring_bytes{0};
    uint16_t m_queue_num{0};

    Vring m_q{};
};
