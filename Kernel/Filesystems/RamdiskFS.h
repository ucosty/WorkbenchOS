// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <Devices/BlockDevice.h>
#include <Devices/BlockDeviceReader.h>
#include <LibStd/String.h>

namespace RamdiskFS {

#define MAGIC 0x017DCE23

enum class ErrorCode {
    InvalidSuperblockMagic,
    InvalidInodeNumber
};

class Error {
public:
    Error(ErrorCode code) : m_code(code) {}
    Error(BlockDeviceError blockDeviceError) : m_block_device_error(blockDeviceError) {}
private:
    ErrorCode m_code;
    Std::Optional<BlockDeviceError> m_block_device_error;
};

struct Superblock {
    u32 magic;// 0x017DCE23
    u32 version;
    u32 inode_table_offset;
    u32 inode_table_size;
    u32 data_table_offset;
};

enum class DirectoryEntryType : u16 {
    Unknown = 0,
    File = 1,
    Directory = 2,
};

struct DirectoryEntry {
    u16 type;
    u32 filename_inode;
    u32 data_inode;
};

struct Directory {
    u32 entry_count;
};

struct Inode {
    u32 offset;
    u32 size;
};

struct Entry {
    Std::String name;
    DirectoryEntryType type;
    u32 data_inode;
};

class OpenFile;

class Filesystem {
public:
    explicit Filesystem(BlockDevice *block_device);

    Std::Result<void> init();

    Std::Result<OpenFile> open(const Std::String &filename);

    Std::Result<Std::Vector<Entry>> read_directory(BlockDeviceReader &reader, u32 directory_inode_number);

    Std::Result<void> read(u8 *buffer, Inode inode, size_t offset, size_t size);

private:
    Std::Result<u32> find_file_in_directory(u32 directory_inode_index, const Std::String &filename);

    Std::Result<Std::String> read_filename_inode(BlockDeviceReader &reader, u32 index);

    Std::Result<Inode> read_inode(BlockDeviceReader &reader, u32 index) const;

    Std::Result<u32> find_file(BlockDeviceReader &reader, Std::String filename);

    BlockDevice *m_block_device;

    Superblock m_superblock{};

    static size_t get_directory_entry_offset(size_t base, size_t index);

    Std::Result<u32> find_directory_containing_file(Std::String path);
};

class OpenFile {
public:
    OpenFile(Inode inode, Filesystem *filesystem) : m_inode(inode), m_filesystem(filesystem) {}

    Std::Result<void> read(u8 *buffer, size_t size);

    size_t size() const { return m_inode.size; }

private:
    Inode m_inode;
    size_t m_read_offset{0};
    Filesystem *m_filesystem;
};

};// namespace RamdiskFS
