// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include <Debugging.h>
#include <Devices/BlockDeviceReader.h>
#include <Filesystems/SquashFS.h>
#include "LibStd/StringView.h"
#include "LibStd/Try.h"
#include <LibStd/Vector.h>

using namespace Std;

namespace SquashFS {
Filesystem::Filesystem(BlockDevice *block_device) : m_block_device(block_device) {}

const char *compression_type[] = {
    "1 - GZIP",
    "2 - LZMA",
    "3 - LZO",
    "4 - XZ",
    "5 - LZ4",
    "6 - ZSTD",
};

const char *inode_types[] = {
    "Basic Directory",
    "Basic File",
    "Basic Symlink",
    "Basic Block Device",
    "Basic Char Device",
    "Basic Fifo",
    "Basic Socket",
    "Extended Directory",
    "Extended File",
    "Extended Symlink",
    "Extended Block Device",
    "Extended Char Device",
    "Extended Fifo",
    "Extended Socket",
};

Result<void> Filesystem::init() {
    println("SquashFS::Filesystem::init()");
    VERIFY(m_block_device != nullptr);

    m_block_device->read(0, sizeof(Superblock), reinterpret_cast<u8 *>(&m_superblock));

    if (m_superblock.magic != 0x73717368) {
        println("SquashFS: Invalid magic {}", m_superblock.magic);
        return Error::from_code(1);
    }

    println("SquashFS: Compressor = {}", compression_type[m_superblock.compression_id - 1]);
    println("SquashFS: Inode Table Start = {}", m_superblock.inode_table_start);
    println("SquashFS: Directory Table Start = {}", m_superblock.directory_table_start);
    println("SquashFS: Root directory inode = {}, offset_bytes = {}", m_superblock.root_inode_ref, m_superblock.root_inode_ref * m_superblock.block_size);

    if (m_superblock.flags & Flags::UncompressedData) {
        println("SquashFS: Data compression disabled");
    }
    if (m_superblock.flags & Flags::UncompressedFragments) {
        println("SquashFS: Fragments compression disabled");
    }
    if (m_superblock.flags & Flags::UncompressedIds) {
        println("SquashFS: Ids compression disabled");
    }
    if (m_superblock.flags & Flags::UncompressedInodes) {
        println("SquashFS: Inodes compression disabled");
    }
    if (m_superblock.flags & Flags::UncompressedXattrs) {
        println("SquashFS: Xattrs compression disabled");
    }
    //    auto reader = BlockDeviceReader(m_block_device);

    println("Listing root directory...");
    TRY(list(m_superblock.directory_table_start));

    //    u8 buffer[12] = {0};

    //    TRY(reader.seek(m_superblock.directory_table_start));
    //    u16 metadata_header = TRY(reader.read_uint16());
    //    auto compressed = !(metadata_header & 0x8000);
    //    auto data_size = metadata_header & 0x7FFF;
    //
    //    if (!compressed) {
    //        printf("Directory Table: Compression = off, size = %d\n", data_size);
    //    }
    //
    //    m_block_device->read(m_superblock.directory_table_start + 2, 12, buffer);
    //
    //    auto directory_header = reinterpret_cast<DirectoryHeader *>(buffer);
    //    printf("Directory: count = %x, start = %x, inode_number = %x\n", directory_header->count + 1, directory_header->start, directory_header->inode_number);
    //
    //    int offset = 0;
    //    for (int i = 0; i < directory_header->count + 1; i++) {
    //        u8 directory_entry_buffer[8] = {0};
    //        m_block_device->read(m_superblock.directory_table_start + 2 + 12 + offset, 8, directory_entry_buffer);
    //        auto directory_entry = reinterpret_cast<DirectoryEntry *>(directory_entry_buffer);
    //
    //        char *filename = new char[directory_entry->name_size + 2];
    //        memset(filename, 0, directory_entry->name_size + 2);
    //        m_block_device->read(m_superblock.directory_table_start + 2 + 12 + 8 + offset, directory_entry->name_size + 1, reinterpret_cast<u8 *>(filename));
    //        printf("Entry: offset = %x, inode_offset = %x, type = %x, filename = %s\n", directory_entry->offset, directory_entry->inode_offset, directory_entry->type, filename);
    //
    //        offset += directory_entry->name_size + 1 + 8;
    //        delete[] filename;
    //
    //        // Hack to get test.txt file
    //        if (directory_entry->inode_offset == 3) {
    //            // Read the inode
    //            u8 inode_header_buffer[sizeof(CommonInodeHeader)];
    //            TRY(reader.seek(m_superblock.inode_table_start + directory_entry->offset + 2));
    //            TRY(reader.read_buffer(inode_header_buffer, sizeof(CommonInodeHeader)));
    //
    //            auto inode_header = reinterpret_cast<CommonInodeHeader *>(&inode_header_buffer);
    //
    //            if (inode_header->type == 2) {
    //                u8 file_inode_buffer[sizeof(FileInode)];
    //                TRY(reader.seek(m_superblock.inode_table_start + directory_entry->offset + 2));
    //                TRY(reader.read_buffer(file_inode_buffer, sizeof(FileInode)));
    //                auto file_inode = reinterpret_cast<FileInode *>(&file_inode_buffer);
    //
    //                printf("File: block_offset = %x, file_offset = %x, size = %d\n", file_inode->block_offset, file_inode->block_offset + sizeof(Superblock), file_inode->file_size);
    //
    //                auto file_offset = file_inode->block_offset + sizeof(Superblock);
    //                auto file_buffer = new u8[file_inode->file_size + 1];
    //                memset(reinterpret_cast<char *>(file_buffer), 0, file_inode->file_size + 1);
    //                TRY(reader.seek(file_offset));
    //                TRY(reader.read_buffer(file_buffer, file_inode->file_size));
    //                printf("File:\n%s\n", file_buffer);
    //            }
    //        }
    //    }
    //    TRY(find_file_by_filename("testapp"_sv));
    return {};
}

Result<void> read_next_directory_entry(BlockDeviceReader *reader) {
    auto directory_entry = TRY(reader->read<DirectoryEntryHeader>());
    auto name = TRY(reader->read_string(directory_entry.name_size + 1));
    println("\nDirectory Entry: type = {}, offset = {}, name = {}", inode_types[directory_entry.type - 1], directory_entry.offset, &name);
    return {};
}

// Assume root directory for now
Result<void> Filesystem::list(size_t directory_start) {
    println("Filesystem::list(): Listing root directory...");
    auto reader = BlockDeviceReader(m_block_device);
    TRY(reader.seek(directory_start));

    auto metablock_header = TRY(reader.read_uint16());
    println("metablock_header = {}", metablock_header);

    auto directory_header = TRY(reader.read<DirectoryHeader>());
    println("directory_header.count = {}", directory_header.count);
    println("directory_header.inode_number = {}", directory_header.inode_number);
    println("directory_header.start = {}", directory_header.start);


    println("Filesystem::list(): reading {} entries", directory_header.count + 1);
    for (int i = 0; i < directory_header.count + 1; i++) {
        println("Reading directory entry {}...", i);
        TRY(read_next_directory_entry(&reader));
    }
    println("Filesystem::list(): done!");
    return {};
}

Result<OpenFile> Filesystem::open(StringView filename) {
    auto inode = TRY(find_file_by_filename(filename));
    return OpenFile(inode);
}

Result<FileInode> Filesystem::find_file_by_filename(StringView filename) {
    auto parts = filename.split('/');
    int depth = 0;
    size_t max_depth = parts.length() - 1;
    while (depth < max_depth) {
        depth++;
    }

    return find_file_in_directory(parts[max_depth], m_superblock.directory_table_start);
}

Result<FileInode> Filesystem::find_file_in_directory(StringView filename, size_t directory_start) {
    auto directory_header = TRY(read_directory_header(directory_start));
    println("find_file_in_directory(): Directory count = {}, start = {}, inode_number = {}", directory_header.count + 1, directory_header.start, directory_header.inode_number);

    size_t directory_entry_offset = directory_start + sizeof(u16) + sizeof(DirectoryHeader);
    DirectoryEntryHeader directory_entry{};

    for (int i = 0; i < directory_header.count + 1; i++) {
        // Read the directory entry metadata
        m_block_device->read(directory_entry_offset, sizeof(DirectoryEntryHeader), reinterpret_cast<u8 *>(&directory_entry));

        // Read the filename
        size_t name_length = directory_entry.name_size + 1;
        char *directory_entry_name = new char[name_length];
        m_block_device->read(directory_entry_offset + sizeof(DirectoryEntryHeader), name_length, reinterpret_cast<u8 *>(directory_entry_name));

        auto name = StringView(directory_entry_name, name_length);
        if (name == filename) {
            delete[] directory_entry_name;
            //            return directory_entry.offset;
            return FileInode();
        }

        directory_entry_offset += sizeof(DirectoryEntryHeader) + name_length;
        delete[] directory_entry_name;
    }

    println("File not found :(");
    return FileInode();
}

Result<DirectoryHeader> Filesystem::read_directory_header(size_t directory_start) {
    DirectoryHeader directory_header;
    m_block_device->read(directory_start + sizeof(u16), sizeof(DirectoryHeader), reinterpret_cast<u8 *>(&directory_header));
    return directory_header;
}

}// namespace SquashFS
