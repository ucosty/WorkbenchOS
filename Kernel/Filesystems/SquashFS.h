// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <Devices/BlockDevice.h>

namespace SquashFS {

struct PACKED Superblock {
    uint32_t magic;
    uint32_t inode_count;
    uint32_t modification_time;
    uint32_t block_size;
    uint32_t fragment_entry_count;
    uint16_t compression_id;
    uint16_t block_log;
    uint16_t flags;
    uint16_t id_count;
    uint16_t version_major;
    uint16_t version_minor;
    uint64_t root_inode_ref;
    uint64_t bytes_used;
    uint64_t id_table_start;
    uint64_t xattr_id_table_start;
    uint64_t inode_table_start;
    uint64_t directory_table_start;
    uint64_t fragment_table_start;
    uint64_t export_table_start;
};

enum Flags : uint16_t {
    UncompressedInodes = 0x00001,
    UncompressedData = 0x00002,
    Check = 0x0004,
    UncompressedFragments = 0x0008,
    NoFragments = 0x0010,
    AlwaysFragments = 0x0020,
    Duplicates = 0x0040,
    Exportable = 0x0080,
    UncompressedXattrs = 0x0100,
    NoXattrs = 0x0200,
    CompressorOptions = 0x0400,
    UncompressedIds = 0x0800,
};

struct DirectoryHeader {
    uint32_t count{0};
    uint32_t start{0};
    uint32_t inode_number{0};
};

struct DirectoryEntry {
    uint16_t offset;
    uint16_t inode_offset;
    uint16_t type;
    uint16_t name_size;
    char *name;
};

struct PACKED CommonInodeHeader {
    uint16_t type;
    uint16_t permissions;
    uint16_t uid;
    uint16_t gid;
    uint32_t mtime;
    uint32_t inode_number;
};

struct PACKED DirectoryInode {
    CommonInodeHeader header;
    uint32_t block_index;
    uint32_t link_count;
    uint16_t file_size;
    uint16_t block_offset;
    uint32_t parent_inode;
};

struct PACKED FileInode {
    CommonInodeHeader header;
    uint32_t blocks_start;
    uint32_t frag_index;
    uint32_t block_offset;
    uint32_t file_size;
    uint32_t block_sizes;
};

static_assert(sizeof(FileInode) == 0x24);

class Filesystem {
public:
    Filesystem(BlockDevice *block_device);

    Result<void> init();

private:
    BlockDevice *m_block_device;
    Superblock m_superblock;

    DirectoryEntry read_directory_entry();
};

}// namespace SquashFS
