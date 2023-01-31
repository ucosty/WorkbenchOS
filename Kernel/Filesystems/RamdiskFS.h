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
    uint32_t magic;// 0x017DCE23
    uint32_t version;
    uint32_t inode_table_offset;
    uint32_t inode_table_size;
    uint32_t data_table_offset;
};

enum class DirectoryEntryType : uint16_t {
    Unknown = 0,
    File = 1,
    Directory = 2,
};

struct DirectoryEntry {
    uint16_t type;
    uint32_t filename_inode;
    uint32_t data_inode;
};

struct Directory {
    uint32_t entry_count;
};

struct Inode {
    uint32_t offset;
    uint32_t size;
};

struct Entry {
    Std::String name;
    DirectoryEntryType type;
    uint32_t data_inode;
};

class OpenFile;

class Filesystem {
public:
    explicit Filesystem(BlockDevice *block_device);

    Std::Result<void> init();

    Std::Result<OpenFile> open(const Std::String &filename);

    Std::Result<Std::Vector<Entry>> read_directory(BlockDeviceReader &reader, uint32_t directory_inode_number);

    Std::Result<void> read(uint8_t *buffer, Inode inode, size_t offset, size_t size);

private:
    Std::Result<uint32_t> find_file_in_directory(uint32_t directory_inode_index, const Std::String &filename);

    Std::Result<Std::String> read_filename_inode(BlockDeviceReader &reader, uint32_t index);

    Std::Result<Inode> read_inode(BlockDeviceReader &reader, uint32_t index) const;

    Std::Result<uint32_t> find_file(BlockDeviceReader &reader, Std::String filename);

    BlockDevice *m_block_device;

    Superblock m_superblock{};

    static size_t get_directory_entry_offset(size_t base, size_t index);

    Std::Result<uint32_t> find_directory_containing_file(Std::String path);
};

class OpenFile {
public:
    OpenFile(Inode inode, Filesystem *filesystem) : m_inode(inode), m_filesystem(filesystem) {}

    Std::Result<void> read(uint8_t *buffer, size_t size);

    size_t size() const { return m_inode.size; }

private:
    Inode m_inode;
    size_t m_read_offset{0};
    Filesystem *m_filesystem;
};

};// namespace RamdiskFS
