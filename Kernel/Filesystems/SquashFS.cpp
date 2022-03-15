// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include <ConsoleIO.h>
#include <Debugging.h>
#include <Filesystems/SquashFS.h>
#include <Try.h>
#include <cstring.h>

class BlockDeviceReader {
public:
    BlockDeviceReader(BlockDevice *block_device) : m_block_device(block_device) {}

    Result<uint16_t> read_uint16() {
        uint16_t result;
        m_block_device->read(m_offset, sizeof(uint16_t), reinterpret_cast<uint8_t *>(&result));
        m_offset += sizeof(uint16_t);
        return result;
    }

    Result<void> read_buffer(uint8_t *destination, size_t size) {
        m_block_device->read(m_offset, size, destination);
        m_offset += size;
        return {};
    }

    Result<void> seek(size_t offset) {
        m_offset = offset;
        return {};
    }

private:
    BlockDevice *m_block_device;
    size_t m_offset{0};
};

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

Result<void> Filesystem::init() {
    printf("SquashFS::Filesystem::init()\n");
    VERIFY(m_block_device != nullptr);

    m_block_device->read(0, sizeof(Superblock), reinterpret_cast<uint8_t *>(&m_superblock));

    if (m_superblock.magic != 0x73717368) {
        printf("SquashFS: Invalid magic %x\n", m_superblock.magic);
        return Lib::Error::from_code(1);
    }

    printf("SquashFS: Compressor = %s\n", compression_type[m_superblock.compression_id - 1]);
    printf("SquashFS: Inode Table Start = %X\n", m_superblock.inode_table_start);
    printf("SquashFS: Directory Table Start = %X\n", m_superblock.directory_table_start);

    if (m_superblock.flags & Flags::UncompressedData) {
        printf("SquashFS: Data compression disabled\n");
    }
    if (m_superblock.flags & Flags::UncompressedFragments) {
        printf("SquashFS: Fragments compression disabled\n");
    }
    if (m_superblock.flags & Flags::UncompressedIds) {
        printf("SquashFS: Ids compression disabled\n");
    }
    if (m_superblock.flags & Flags::UncompressedInodes) {
        printf("SquashFS: Inodes compression disabled\n");
    }
    if (m_superblock.flags & Flags::UncompressedXattrs) {
        printf("SquashFS: Xattrs compression disabled\n");
    }
    auto reader = BlockDeviceReader(m_block_device);

    uint8_t buffer[12] = {0};

    TRY(reader.seek(m_superblock.directory_table_start));
    uint16_t metadata_header = TRY(reader.read_uint16());
    auto compressed = !(metadata_header & 0x8000);
    auto data_size = metadata_header & 0x7FFF;

    if (!compressed) {
        printf("Directory Table: Compression = off, size = %d\n", data_size);
    }

    m_block_device->read(m_superblock.directory_table_start + 2, 12, buffer);

    auto directory_header = reinterpret_cast<DirectoryHeader *>(buffer);
    printf("Directory: count = %x, start = %x, inode_number = %x\n", directory_header->count + 1, directory_header->start, directory_header->inode_number);

    int offset = 0;
    for (int i = 0; i < directory_header->count + 1; i++) {
        uint8_t directory_entry_buffer[8] = {0};
        m_block_device->read(m_superblock.directory_table_start + 2 + 12 + offset, 8, directory_entry_buffer);
        auto directory_entry = reinterpret_cast<DirectoryEntry *>(directory_entry_buffer);

        char *filename = new char[directory_entry->name_size + 2];
        memset(filename, 0, directory_entry->name_size + 2);
        m_block_device->read(m_superblock.directory_table_start + 2 + 12 + 8 + offset, directory_entry->name_size + 1, reinterpret_cast<uint8_t *>(filename));
        printf("Entry: offset = %x, inode_offset = %x, type = %x, filename = %s\n", directory_entry->offset, directory_entry->inode_offset, directory_entry->type, filename);

        offset += directory_entry->name_size + 1 + 8;
        delete[] filename;

        // Hack to get test.txt file
        if(directory_entry->inode_offset == 3) {
            // Read the inode
            uint8_t inode_header_buffer[sizeof(CommonInodeHeader)];
            TRY(reader.seek(m_superblock.inode_table_start + directory_entry->offset + 2));
            TRY(reader.read_buffer(inode_header_buffer, sizeof(CommonInodeHeader)));

            auto inode_header = reinterpret_cast<CommonInodeHeader *>(&inode_header_buffer);

            if(inode_header->type == 2) {
                uint8_t file_inode_buffer[sizeof(FileInode)];
                TRY(reader.seek(m_superblock.inode_table_start + directory_entry->offset + 2));
                TRY(reader.read_buffer(file_inode_buffer, sizeof(FileInode)));
                auto file_inode = reinterpret_cast<FileInode *>(&file_inode_buffer);

                printf("File: block_offset = %x, file_offset = %x, size = %d\n", file_inode->block_offset, file_inode->block_offset + sizeof(Superblock), file_inode->file_size);

                auto file_offset = file_inode->block_offset + sizeof(Superblock);
                auto file_buffer = new uint8_t[file_inode->file_size + 1];
                memset(reinterpret_cast<char *>(file_buffer), 0, file_inode->file_size + 1);
                TRY(reader.seek(file_offset));
                TRY(reader.read_buffer(file_buffer, file_inode->file_size));
                printf("File:\n%s\n", file_buffer);
            }
        }
    }



    return {};
}


}// namespace SquashFS
