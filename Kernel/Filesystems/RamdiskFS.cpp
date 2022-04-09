// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include <ConsoleIO.h>
#include <Devices/BlockDeviceReader.h>
#include <Filesystems/RamdiskFS.h>
#include <LibStd/String.h>
#include <LibStd/Try.h>

using namespace Std;

namespace RamdiskFS {

Filesystem::Filesystem(BlockDevice *block_device) : m_block_device(block_device) {}

Result<void> Filesystem::init() {
    auto reader = BlockDeviceReader(m_block_device);
    m_superblock = TRY(reader.read<Superblock>());
    if (m_superblock.magic != MAGIC) {
        return Error::from_code(1);
    }
    return {};
}

Result<OpenFile> Filesystem::open(StringView filename) {
    auto inode = Inode{};
    return OpenFile{inode};
}

Result<Inode> Filesystem::read_inode(BlockDeviceReader &reader, uint32_t index) {
    if (index > m_superblock.inode_table_size) {
        printf("Invalid inode: %d\n", index);
    }

    uint32_t inode_offset = sizeof(Superblock) + (sizeof(Inode) * index);
    reader.seek(inode_offset);
    return reader.read<Inode>();
}

Result<String> Filesystem::read_filename_inode(BlockDeviceReader &reader, uint32_t index) {
    auto inode = TRY(read_inode(reader, index));
    auto *filename = new char[inode.size];
    reader.seek(m_superblock.data_table_offset + inode.offset);
    reader.read_buffer(reinterpret_cast<uint8_t *>(filename), inode.size);
    return String(filename, inode.size);
}

Result<uint32_t> Filesystem::find_file_in_directory(uint32_t directory_inode_index, StringView filename) {
    auto reader = BlockDeviceReader(m_block_device);
    auto directory_inode = TRY(read_inode(reader, directory_inode_index));

    reader.seek(m_superblock.data_table_offset + directory_inode.offset);
    auto header = TRY(reader.read<Directory>());

    for (int i = 0; i < header.entry_count; i++) {
        reader.seek(m_superblock.data_table_offset + directory_inode.offset + sizeof(Directory) + (sizeof(DirectoryEntry) * i));
        auto directory_entry = TRY(reader.read<DirectoryEntry>());
        auto entry_name = TRY(read_filename_inode(reader, directory_entry.filename_inode));
        if (entry_name == filename) {
            return directory_entry.data_inode;
        }
    }

    return Error::from_code(1);
}

Result<uint32_t> Filesystem::find_file(BlockDeviceReader &reader, StringView filename) {
    auto parts = filename.split('/');
    int depth = 0;

    uint32_t directory_inode = 0;
    while (depth < parts.length()) {
        directory_inode = TRY(find_file_in_directory(directory_inode, parts[depth]));
        depth++;
    }

    return directory_inode;
}

}// namespace RamdiskFS
