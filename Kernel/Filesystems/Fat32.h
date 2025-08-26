// WorkbenchOS
// Copyright (c) 2025 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once
#include <LibStd/Types.h>

#include <Devices/BlockDevice.h>

enum class Attributes : u8 {
    READONLY = 0x01,
    HIDDEN = 0x02,
    SYSTEM = 0x04,
    VOLUME = 0x08,
    DIR = 0x10,
    ARCH = 0x20,
    LFN = 0x0F
};

struct PACKED BPB_FAT32 {
    uint8_t jmp[3];
    char oem[8];
    uint16_t BytsPerSec; // 11
    uint8_t SecPerClus; // 13
    uint16_t RsvdSecCnt; // 14
    uint8_t NumFATs; // 16
    uint16_t RootEntCnt; // 17 (0 for FAT32)
    uint16_t TotSec16; // 19
    uint8_t Media; // 21
    uint16_t FATSz16; // 22
    uint16_t SecPerTrk; // 24
    uint16_t NumHeads; // 26
    uint32_t HiddSec; // 28
    uint32_t TotSec32; // 32
    uint32_t FATSz32; // 36
    uint16_t ExtFlags; // 40
    uint16_t FSVer; // 42
    uint32_t RootClus; // 44
    uint16_t FSInfo; // 48
    uint16_t BkBootSec; // 50
    uint8_t Reserved[12]; // 52
    uint8_t DrvNum; // 64
    uint8_t Reserved1; // 65
    uint8_t BootSig; // 66
    uint32_t VolID; // 67
    char VolLab[11]; // 71
    char FilSysType[8]; // 82 ("FAT32   ")
    // ... boot code ... ; signature 0x55AA at offset 510
};

struct PACKED DirEntShort {
    char Name[11]; // 8.3 name, space-padded
    uint8_t Attr; // 0x10 dir, 0x20 arch, 0x08 volume, 0x0F LFN
    uint8_t NTRes;
    uint8_t CrtTimeTenth;
    uint16_t CrtTime, CrtDate, LstAccDate;
    uint16_t FstClusHI;
    uint16_t WrtTime, WrtDate;
    uint16_t FstClusLO;
    uint32_t FileSize;
};

class Fat32 {
public:
    Std::Result<void> mount(BlockDevice *dev, uint64_t partition_lba);

    // Read whole file by absolute path like "/EFI/BOOT/BOOTX64.EFI"
    // Returns number of bytes read, or <0 on error.
    Std::Result<u64> read_file(const char *abs_path, void *out_buf, uint32_t max_bytes) const;

private:
    // helpers
    Std::Result<u32> read_fat(uint32_t cluster) const; // returns next cluster (28-bit masked)
    Std::Result<void> read_cluster(uint32_t cluster, void *buf) const;

    Std::Result<void> walk_path(const char *path, /*out*/ DirEntShort &out_ent) const;

    Std::Result<void> find_in_directory(uint32_t dir_first_cluster,
                           const char *name_8dot3_upper, /*out*/ DirEntShort &out_ent) const;

    // utilities
    static void to_8dot3_upper(const char *segment, char out11[11]);

    static bool name_matches(const DirEntShort &e, const char want11[11]);

    BlockDevice *m_dev{};
    uint64_t m_part_lba{};
    uint32_t m_bytes_per_sec{};
    uint32_t m_sec_per_clus{};
    uint32_t m_reserved_sec{};
    uint32_t m_num_fats{};
    uint32_t m_sectors_per_fat{};
    uint32_t m_root_cluster{};
    uint64_t m_fat_begin_lba{};
    uint64_t m_data_begin_lba{};
};
