// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include "LibStd/StringSplitter.h"
#include <Devices/BlockDeviceReader.h>
#include <Filesystems/RamdiskFS.h>
#include <LibStd/String.h>
#include <LibStd/Try.h>
#include <LibStd/Vector.h>
#include <UnbufferedConsole.h>
#include <ConsoleIO.h>
using namespace Std;
void debug_putchar(char c);

namespace RamdiskFS {

Filesystem::Filesystem(BlockDevice *block_device) : m_block_device(block_device) {}

Result<void> Filesystem::init() {
    auto reader = BlockDeviceReader(m_block_device);
    m_superblock = TRY(reader.read<Superblock>());
    if (m_superblock.magic != MAGIC) {
        return Std::Error::with_message("RamdiskFS: Invalid superblock"_sv);//ErrorCode::InvalidSuperblockMagic);
    }
    return {};
}

Result<OpenFile> Filesystem::open(const String &filename) {
    auto reader = BlockDeviceReader(m_block_device);
    auto inode_number = TRY(find_file(reader, filename));
    auto inode = TRY(read_inode(reader, inode_number));
    return OpenFile{inode, this};
}

Result<Inode> Filesystem::read_inode(BlockDeviceReader &reader, u32 index) const {
    if (index > m_superblock.inode_table_size) {
        return Std::Error::from_code(1);//ErrorCode::InvalidInodeNumber);
    }

    u32 inode_offset = sizeof(Superblock) + (sizeof(Inode) * index);
    reader.seek(inode_offset);
    return TRY(reader.read<Inode>());
}

Result<String> Filesystem::read_filename_inode(BlockDeviceReader &reader, u32 index) {
    auto inode = TRY(read_inode(reader, index));
    auto *filename = new char[inode.size];
    reader.seek(m_superblock.data_table_offset + inode.offset);
    reader.read_buffer(reinterpret_cast<u8 *>(filename), inode.size);
    return String(filename, inode.size);
}

Result<u32> Filesystem::find_file_in_directory(u32 directory_inode_index, const String &filename) {
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
    return Std::Error::with_message("File not found"_sv);
}

Result<u32> Filesystem::find_file(BlockDeviceReader &reader, String filename) {
    auto directory = TRY(find_directory_containing_file(filename));
    auto file = TRY(find_file_in_directory(directory, filename));
    return file;
}

Std::Result<u32> Filesystem::find_directory_containing_file(String path) {
    auto path_parts = StringSplitter::split(path, '/');
    auto max_search_depth = path_parts.length() - 1;
    auto current_search_depth = 0;
    auto inode = 0;

    while (current_search_depth < max_search_depth) {
        println("Looking in {}", path_parts[current_search_depth]);
        inode = TRY(find_file_in_directory(inode, path_parts[current_search_depth]));
        max_search_depth++;
    }

    return inode;
}

Result<Vector<Entry>> Filesystem::read_directory(BlockDeviceReader &reader, u32 directory_inode_number) {
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

Std::Result<void> Filesystem::read(u8 *buffer, Inode inode, size_t offset, size_t size) {
    if (buffer == nullptr) {
        return Std::Error::from_code(1);
    }
    auto reader = BlockDeviceReader(m_block_device);
    TRY(reader.seek(m_superblock.data_table_offset + inode.offset + offset));
    TRY(reader.read_buffer(buffer, size));
    return {};
}

Std::Result<void> OpenFile::read(u8 *buffer, size_t size) {
    TRY(m_filesystem->read(buffer, m_inode, m_read_offset, size));
    m_read_offset += size;
    return {};
}
}// namespace RamdiskFS
