// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <Devices/BlockDevice.h>
#include <Devices/BlockDeviceReader.h>
#include "LibStd/StringView.h"

namespace RamdiskFS {

#define MAGIC 0x017DCE23

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

class OpenFile {
public:
    explicit OpenFile(Inode inode) : m_inode(inode) {}

private:
    Inode m_inode;
    size_t m_read_offset{0};
};

class Filesystem {
public:
    explicit Filesystem(BlockDevice *block_device);

    Std::Result<void> init();

    Std::Result<OpenFile> open(Std::StringView filename);

private:
    Std::Result<uint32_t> find_file_in_directory(uint32_t directory_inode_index, Std::StringView filename);

    Std::Result<Std::String> read_filename_inode(BlockDeviceReader &reader, uint32_t index);

    Std::Result<Inode> read_inode(BlockDeviceReader &reader, uint32_t index);

    Std::Result<uint32_t> find_file(BlockDeviceReader &reader, Std::StringView filename);

    BlockDevice *m_block_device;

    Superblock m_superblock{};
};
};// namespace RamdiskFS
