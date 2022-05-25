// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include <Devices/BlockDeviceReader.h>
#include <Filesystems/RamdiskFS.h>
#include <LibStd/String.h>
#include <LibStd/Try.h>
#include <LibStd/Vector.h>
#include <UnbufferedConsole.h>
#include "LibStd/StringSplitter.h"

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

Result<OpenFile> Filesystem::open(const String& filename) {
    auto reader = BlockDeviceReader(m_block_device);
    println("Looking for file: {}", filename);
    auto inode_number = TRY(find_file(reader, filename));
    println("Found inode: {}", inode_number);
    auto inode = TRY(read_inode(reader, inode_number));
    println("Read inode: offset = {}, size = {}", inode.offset, inode.size);
    return OpenFile{inode};
}

Result<Inode> Filesystem::read_inode(BlockDeviceReader &reader, uint32_t index) const {
    if (index > m_superblock.inode_table_size) {
        println("Invalid inode: {}", index);
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

Result<uint32_t> Filesystem::find_file_in_directory(uint32_t directory_inode_index, const String& filename) {
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

Result<uint32_t> Filesystem::find_file(BlockDeviceReader &reader, String filename) {
    auto parts = StringSplitter::split(filename, '/');
    int depth = 0;

    uint32_t directory_inode = 0;
    while (depth < parts.length()) {
        println("Looking for {} in {}", filename, parts[depth]);
        directory_inode = TRY(find_file_in_directory(directory_inode, parts[depth]));
        depth++;
    }

    return directory_inode;
}

Result<Vector<Entry>> Filesystem::read_directory(BlockDeviceReader &reader, uint32_t directory_inode_number) {
    Vector<Entry> result;
    auto directory_inode = TRY(read_inode(reader, directory_inode_number));
    auto directory_offset = m_superblock.data_table_offset + directory_inode.offset;
    reader.seek(directory_offset);
    auto directory = TRY(reader.read<Directory>());
    for (int i = 0; i < directory.entry_count; i++) {
        auto directory_entry_offset = get_directory_entry_offset(m_superblock.data_table_offset + directory_inode.offset + sizeof(Directory), i);
        reader.seek(directory_entry_offset);
        auto directory_entry = TRY(reader.read<DirectoryEntry>());
        auto entry_name = TRY(read_filename_inode(reader, directory_entry.filename_inode));
        auto entry = Entry{entry_name, static_cast<DirectoryEntryType>(directory_entry.type), directory_entry.data_inode};
        result.append(entry);
    }
    return result;
}

size_t Filesystem::get_directory_entry_offset(size_t base, size_t index) {
    return base + (sizeof(DirectoryEntry) * index);
}

}// namespace RamdiskFS
