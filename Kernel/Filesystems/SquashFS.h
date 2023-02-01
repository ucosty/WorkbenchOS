// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <Devices/BlockDevice.h>
#include "LibStd/StringView.h"

namespace SquashFS {

struct PACKED Superblock {
    u32 magic;
    u32 inode_count;
    u32 modification_time;
    u32 block_size;
    u32 fragment_entry_count;
    u16 compression_id;
    u16 block_log;
    u16 flags;
    u16 id_count;
    u16 version_major;
    u16 version_minor;
    u64 root_inode_ref;
    u64 bytes_used;
    u64 id_table_start;
    u64 xattr_id_table_start;
    u64 inode_table_start;
    u64 directory_table_start;
    u64 fragment_table_start;
    u64 export_table_start;
};

enum Flags : u16 {
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

enum InodeType : u16 {
    BasicDirectory = 1,
    BasicFile = 2,
    BasicSymlink = 3,
    BasicBlockDevice = 4,
    BasicCharDevice = 5,
    BasicFifo = 6,
    BasicSocket = 7,
    ExtendedDirectory = 8,
    ExtendedFile = 9,
    ExtendedSymlink = 10,
    ExtendedBlockDevice = 11,
    ExtendedCharDevice = 12,
    ExtendedFifo = 13,
    ExtendedSocket = 14,
};

struct DirectoryHeader {
    u32 count{0};
    u32 start{0};
    u32 inode_number{0};
};

struct DirectoryEntryHeader {
    u16 offset;
    u16 inode_offset;
    u16 type;
    u16 name_size;
};

struct DirectoryEntry {
    DirectoryEntryHeader header;
    char *name;
};

static_assert(sizeof(DirectoryEntryHeader) == 8);

struct PACKED CommonInodeHeader {
    u16 type;
    u16 permissions;
    u16 uid;
    u16 gid;
    u32 mtime;
    u32 inode_number;
};

struct PACKED DirectoryInode {
    CommonInodeHeader header;
    u32 block_index;
    u32 link_count;
    u16 file_size;
    u16 block_offset;
    u32 parent_inode;
};

struct PACKED FileInode {
    CommonInodeHeader header;
    u32 blocks_start;
    u32 frag_index;
    u32 block_offset;
    u32 file_size;
    u32 block_sizes;
};

class OpenFile {
public:
    OpenFile(FileInode inode) : m_inode(inode) {}

private:
    FileInode m_inode;
    size_t m_read_offset{0};
};

static_assert(sizeof(FileInode) == 0x24);

class Filesystem {
public:
    Filesystem(BlockDevice *block_device);

    Std::Result<void> init();

    Std::Result<OpenFile> open(Std::StringView filename);

    Std::Result<void> list(size_t directory_start);

    Std::Result<FileInode> find_file_by_filename(Std::StringView filename);

    Std::Result<FileInode> find_file_in_directory(Std::StringView filename, size_t directory_start);

private:
    BlockDevice *m_block_device;
    Superblock m_superblock;

    Std::Result<DirectoryHeader> read_directory_header(size_t directory_start);
};

}// namespace SquashFS
