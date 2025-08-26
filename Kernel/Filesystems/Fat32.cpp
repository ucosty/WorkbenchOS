// WorkbenchOS
// Copyright (c) 2025 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include <UnbufferedConsole.h>
#include <Filesystems/Fat32.h>
#include <LibStd/CString.h>

#include "Heap/Kmalloc.h"
#include "LibStd/Try.h"

Std::Result<void> Fat32::mount(BlockDevice *dev, u64 partition_lba) {
    m_dev = dev;
    m_part_lba = partition_lba;

    u8 sec[512]; // will resize to actual BPS after reading
    TRY(m_dev->read(partition_lba, 512, sec));

    const BPB_FAT32* bpb = reinterpret_cast<const BPB_FAT32*>(sec);
    const u16 sig = *(const u16*)(sec + 510);
    if (sig != 0xAA55) return Std::Error::from_code(1);

    m_bytes_per_sec   = bpb->BytsPerSec;
    m_sec_per_clus    = bpb->SecPerClus;
    m_reserved_sec    = bpb->RsvdSecCnt;
    m_num_fats        = bpb->NumFATs;
    m_sectors_per_fat = bpb->FATSz32;
    m_root_cluster    = bpb->RootClus;

    if (m_bytes_per_sec < 512 || (m_bytes_per_sec & (m_bytes_per_sec-1))) {
        println("Expecting power of 2 size, got {}", m_bytes_per_sec);
        return Std::Error::from_code(1); // expect power-of-two sector size
    }

    m_fat_begin_lba  = m_part_lba + m_reserved_sec;
    m_data_begin_lba = m_fat_begin_lba + (u64)m_num_fats * m_sectors_per_fat;

    return {};
}

Std::Result<u32> Fat32::read_fat(const u32 cluster) const {
    // FAT32: 4 bytes per entry
    const u64 fat_offset = static_cast<u64>(cluster) * 4ull;
    u64 fat_sec = fat_offset / m_bytes_per_sec;
    const auto off_in_sec = static_cast<u32>(fat_offset % m_bytes_per_sec);

    // Read one FAT sector
    // (You could cache this; fine to re-read for the minimal version.)
    // Use a small stack buffer sized to sector size:
    u8 secbuf[4096]; // supports up to 4K sectors
    TRY(m_dev->read(fat_offset, m_bytes_per_sec, secbuf));

    u32 val = *reinterpret_cast<u32 *>(secbuf + off_in_sec);
    val &= 0x0FFFFFFF; // lower 28 bits are valid
    return val;
}

Std::Result<void> Fat32::read_cluster(const u32 cluster, void* buf) const {
    const u64 first_sec = m_data_begin_lba + static_cast<u64>(cluster - 2) * m_sec_per_clus;

    TRY(m_dev->read(first_sec, m_bytes_per_sec, buf));
    return {};
}

static inline bool is_eoc(u32 c) { return c >= 0x0FFFFFF8u; }

static inline bool is_free(u32 c) { return c == 0; }

void Fat32::to_8dot3_upper(const char* seg, char out11[11]) {
    // Build NAME (8) + EXT (3), space-padded, uppercase.
    memset(out11, ' ', 11);
    // skip leading '/'
    if (*seg == '/' || *seg == '\\') ++seg;
    // split at dot
    const char* dot = nullptr;
    for (const char* p = seg; *p && *p != '/' && *p != '\\'; ++p) if (*p == '.') dot = p;
    const char* end = seg; while (*end && *end != '/' && *end != '\\') ++end;

    // name
    int i = 0;
    for (const char* p = seg; p < end && p != dot && i < 8; ++p) {
        char c = *p;
        if (c >= 'a' && c <= 'z') c -= 32;
        out11[i++] = c;
    }
    // ext
    if (dot && dot+1 < end) {
        int j = 0;
        for (const char* p = dot+1; p < end && j < 3; ++p) {
            char c = *p;
            if (c >= 'a' && c <= 'z') c -= 32;
            out11[8 + j++] = c;
        }
    }
}

bool Fat32::name_matches(const DirEntShort& e, const char want11[11]) {
    if (e.Attr == static_cast<u8>(Attributes::LFN)) return false;
    // if deleted (0xE5) or end (0x00)
    if (static_cast<u8>(e.Name[0]) == 0xE5 || static_cast<u8>(e.Name[0]) == 0x00) return false;
    return memcmp(e.Name, want11, 11) == 0;
}

Std::Result<void> Fat32::find_in_directory(u32 dir_first_cluster,
                                    const char* want11_upper, DirEntShort& out) const
{
    // Walk the cluster chain of this directory, scanning 32B entries
    u32 cl = dir_first_cluster;
    const u32 ents_per_cluster =
        (m_sec_per_clus * m_bytes_per_sec) / sizeof(DirEntShort);


    // temp buffer for one cluster
    u8* clbuf = TRY(g_malloc_heap.allocate(m_sec_per_clus * m_bytes_per_sec)).as_ptr();

    while (!is_eoc(cl) && !is_free(cl)) {
        TRY(read_cluster(cl, clbuf));

        auto* ents = reinterpret_cast<const DirEntShort*>(clbuf);
        for (u32 i = 0; i < ents_per_cluster; ++i) {
            const DirEntShort& e = ents[i];
            const auto first = static_cast<u8>(e.Name[0]);
            if (first == 0x00) return Std::Error::from_code(1); // no more entries
            if (first == 0xE5) continue;     // deleted
            if (e.Attr == static_cast<u8>(Attributes::LFN) || (e.Attr & static_cast<u8>(Attributes::VOLUME))) continue;

            if (memcmp(e.Name, want11_upper, 11) == 0) {
                out = e;
                return {};
            }
        }
        cl = TRY(read_fat(cl));
    }
    return Std::Error::from_code(1);
}

Std::Result<void> Fat32::walk_path(const char* path, DirEntShort& out_ent) const {
    // Absolute path expected. Root is m_root_cluster.
    if (!path || !*path) return Std::Error::from_code(1);
    // Skip leading slashes
    while (*path == '/' || *path == '\\') ++path;

    u32 cur_dir = m_root_cluster;

    // Empty path = root dir (not a file); reject here
    if (*path == 0) return Std::Error::from_code(1);

    // Iterate components
    while (*path) {
        char want11[11];
        // grab [path, next_sep)
        const char* start = path;
        while (*path && *path != '/' && *path != '\\') ++path;
        // build 8.3
        char seg[256];
        const auto len = static_cast<size_t>(path - start);
        if (len >= sizeof(seg)) return Std::Error::from_code(1);
        memcpy(seg, start, len); seg[len] = '\0';
        to_8dot3_upper(seg, want11);

        DirEntShort ent{};
        TRY(find_in_directory(cur_dir, want11, ent));

        // If there is a next component, we must step into a directory
        if (*path == '/' || *path == '\\') {
            if (!(ent.Attr & static_cast<u8>(Attributes::DIR))) return Std::Error::from_code(1);
            u32 hi = ent.FstClusHI, lo = ent.FstClusLO;
            cur_dir = ((u32)hi << 16) | lo;
            // skip separator(s)
            while (*path == '/' || *path == '\\') ++path;
            if (*path == 0) { out_ent = ent; return {}; } // path ended on a dir
        } else {
            // last component; return entry (file or dir)
            out_ent = ent;
            return {};
        }
    }
    return Std::Error::from_code(1);
}

Std::Result<u64> Fat32::read_file(const char* abs_path, void* out_buf, const u32 max_bytes) const {
    DirEntShort ent{};
    TRY(walk_path(abs_path, ent));

    if (ent.Attr & static_cast<u8>(Attributes::DIR)) return Std::Error::from_code(-2);

    const auto file_size = ent.FileSize;
    if (file_size == 0) return 0;

    auto* out = static_cast<u8*>(out_buf);
    u32 to_read = (max_bytes < file_size) ? max_bytes : file_size;

    u32 cl = (static_cast<u32>(ent.FstClusHI) << 16) | ent.FstClusLO;
    const u32 bytes_per_cluster = m_sec_per_clus * m_bytes_per_sec;

    // temp cluster buffer
    u8* clbuf = TRY(g_malloc_heap.allocate(bytes_per_cluster)).as_ptr();

    u32 copied = 0;
    while (to_read > 0 && !is_eoc(cl) && !is_free(cl)) {
        TRY(read_cluster(cl, clbuf));

        const u32 n = (to_read < bytes_per_cluster) ? to_read : bytes_per_cluster;
        memcpy(out + copied, clbuf, n);
        copied += n;
        to_read -= n;

        cl = TRY(read_fat(cl));
    }
    return copied;
}
