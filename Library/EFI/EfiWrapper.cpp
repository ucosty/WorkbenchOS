// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#include <ConsleIO.h>
#include <EFI/EfiWrapper.h>

using namespace EFI;

static const char *STATUS_CODES[] = {
    "EFI_SUCCESS",
    "EFI_LOAD_ERROR",
    "EFI_INVALID_PARAMETER",
    "EFI_UNSUPPORTED",
    "EFI_BAD_BUFFER_SIZE",
    "EFI_BUFFER_TOO_SMALL",
    "EFI_NOT_READY",
    "EFI_DEVICE_ERROR",
    "EFI_WRITE_PROTECTED",
    "EFI_OUT_OF_RESOURCES",
    "EFI_VOLUME_CORRUPTED",
    "EFI_VOLUME_FULL",
    "EFI_NO_MEDIA",
    "EFI_MEDIA_CHANGED",
    "EFI_NOT_FOUND",
    "EFI_ACCESS_DENIED",
    "EFI_NO_RESPONSE",
    "EFI_NO_MAPPING",
    "EFI_TIMEOUT",
    "EFI_NOT_STARTED",
    "EFI_ALREADY_STARTED",
    "EFI_ABORTED",
    "EFI_ICMP_ERROR",
    "EFI_TFTP_ERROR",
    "EFI_PROTOCOL_ERROR",
    "EFI_INCOMPATIBLE_VERSION",
    "EFI_SECURITY_VIOLATION",
    "EFI_CRC_ERROR",
    "EFI_END_OF_MEDIA",
    "EFI_UNDEFINED_29",
    "EFI_UNDEFINED_30",
    "EFI_END_OF_FILE",
    "EFI_INVALID_LANGUAGE",
};

static const char *MEMORY_TYPE[] = {
    "EFI_RESERVED_MEMORY_TYPE",
    "EFI_LOADER_CODE",
    "EFI_LOADER_DATA",
    "EFI_BOOT_SERVICES_CODE",
    "EFI_BOOT_SERVICES_DATA",
    "EFI_RUNTIME_SERVICES_CODE",
    "EFI_RUNTIME_SERVICES_DATA",
    "EFI_CONVENTIONAL_MEMORY",
    "EFI_UNUSABLE_MEMORY",
    "EFI_ACPI_RECLAIM_MEMORY",
    "EFI_ACPI_MEMORY_NVS",
    "EFI_MEMORY_MAPPED_IO",
    "EFI_MEMORY_MAPPED_IO_PORT_SPACE",
    "EFI_PAL_CODE",
    "EFI_PERSISTENT_MEMORY",
};

bool has_status(uint64_t status, uint64_t code) {
    return status &= code;
}

#define EFI_LOAD_ERROR 1
#define EFI_INVALID_PARAMETER 2
#define EFI_UNSUPPORTED 3
#define EFI_BAD_BUFFER_SIZE 4
#define EFI_BUFFER_TOO_SMALL 5
#define EFI_NOT_READY 6
#define EFI_DEVICE_ERROR 7
#define EFI_WRITE_PROTECTED 8
#define EFI_OUT_OF_RESOURCES 9
#define EFI_VOLUME_CORRUPTED 10
#define EFI_VOLUME_FULL 11
#define EFI_NO_MEDIA 12
#define EFI_MEDIA_CHANGED 13
#define EFI_NOT_FOUND 14
#define EFI_ACCESS_DENIED 15
#define EFI_NO_RESPONSE 16
#define EFI_NO_MAPPING 17
#define EFI_TIMEOUT 18
#define EFI_NOT_STARTED 19
#define EFI_ALREADY_STARTED 20
#define EFI_ABORTED 21
#define EFI_ICMP_ERROR 22
#define EFI_TFTP_ERROR 23
#define EFI_PROTOCOL_ERROR 24
#define EFI_INCOMPATIBLE_VERSION 25
#define EFI_SECURITY_VIOLATION 26
#define EFI_CRC_ERROR 27
#define EFI_END_OF_MEDIA 28
#define EFI_END_OF_FILE 31
#define EFI_INVALID_LANGUAGE 32
#define EFI_COMPROMISED_DATA 33
#define EFI_IP_ADDRESS_CONFLICT 34
#define EFI_HTTP_ERROR 35


/**
 * Get a MemoryType in text form
 * @param type memory type to look up
 * @return MemoryType in descriptive form
 */
const char *EFI::memory_type(Raw::MemoryType type) {
    return MEMORY_TYPE[type];
}

/**
 * Get a text representation of an EFI status code
 *
 * @param status code to look up
 * @return status code in descriptive form
 */
const char *EFI::status_code(Raw::Status status) {
    return STATUS_CODES[status];
}

/**
 * Allocates memory pages from the system.
 *
 * @param type The type of allocation to perform
 * @param memory_type The type of memory to allocate
 * @param pages The number of contiguous 4 KiB pages to allocate.
 * @return Pointer to a physical address at the start of the allocated pages
 */
Result<uint64_t> BootServices::allocate_pages(Raw::AllocateType type, Raw::MemoryType memory_type, uint64_t pages) {
    uint64_t value = 0;
    auto status = m_boot_services->allocate_pages(type, memory_type, pages, &value);
    if (status != 0) {
        return Lib::Error::from_code(status);
    }
    return value;
}

/**
 * Allocates pool memory
 *
 * @param pool_type The type of pool to allocate
 * @param size The number of bytes to allocate from the pool
 * @param buffer
 * @return A pointer to a pointer to the allocated buffer if the call succeeds; undefined otherwise
 */
Result<void> BootServices::allocate_pool(Raw::MemoryType pool_type, uint64_t size, void **buffer) {
    if (size == 0) {
        printf("Invalid allocation size = 0\r\n");
        return Lib::Error::from_code(5);
    }
    auto status = m_boot_services->allocate_pool(pool_type, size, buffer);
    if (status != 0) {
        return Lib::Error::from_code(status);
    }
    return {};
}

Result<void> BootServices::free_pool(void *buffer) {
    auto status = m_boot_services->free_pool(buffer);
    if (status != 0) {
        return Lib::Error::from_code(status);
    }
    return {};
}

Result<void> BootServices::locate_protocol(const EFI::GUID *protocol, void *registration, void **interface) {
    auto status = m_boot_services->locate_protocol(protocol, registration, interface);
    if (status != 0) {
        return Lib::Error::from_code(status);
    }
    return {};
}

Result<void> BootServices::set_watchdog_timer(uint64_t timeout, uint64_t watchdog_code, uint64_t data_size, wchar_t *watchdog_data) {
    auto status = m_boot_services->set_watchdog_timer(timeout, watchdog_code, data_size, watchdog_data);
    if (status != 0) {
        return Lib::Error::from_code(status);
    }
    return {};
}

Result<void> BootServices::get_memory_map(uint64_t *memory_map_size, Raw::MemoryDescriptor *memory_map, uint64_t *map_key, uint64_t *descriptor_size, uint32_t *descriptor_version) {
    auto status = m_boot_services->get_memory_map(memory_map_size, memory_map, map_key, descriptor_size, descriptor_version);
    if (status != 0) {
        return Lib::Error::from_code(status);
    }
    return {};
}

Result<MemoryMap> BootServices::get_memory_map() {
    MemoryMap map;
    Raw::Status status = EFI_BUFFER_TOO_SMALL;
    while(true) {
        auto request = get_memory_map(&map.m_size, map.m_descriptors, &map.m_key, &map.m_descriptor_size, &map.m_descriptor_version);
        if(request.is_error()) {
            if (has_status(request.get_error().get_code(), EFI_BUFFER_TOO_SMALL)) {
                if(map.m_descriptors != nullptr) {
                    TRY(free_pool(map.m_descriptors));
                }
                TRY(allocate_pool(EFI::Raw::MemoryType::EFI_LOADER_DATA, map.m_size, (void **) &map.m_descriptors));
            } else {
                return request.get_error();
            }
        } else {
            map.m_descriptor_count = map.m_size / map.m_descriptor_size;
            return map;
        }
    }
}

Result<void> BootServices::exit_boot_services(void *image_handle, uint64_t map_key) {
    auto status = m_boot_services->exit_boot_services(image_handle, map_key);
    if (status != 0) {
        return Lib::Error::from_code(status);
    }
    return {};
}

Result<File> SimpleFileSystemProtcol::open_volume() {
    Raw::File *file;
    auto status = m_filesystem->open_volume(m_filesystem, &file);
    if (status != 0) {
        return Lib::Error::from_code(status);
    }
    return File(m_boot_services, file);
}

Result<File> File::open(const wchar_t *filename, uint64_t mode, uint64_t attributes) {
    Raw::File *opening_file;
    auto status = m_file->open(m_file, &opening_file, filename, mode, attributes);
    if (status != 0) {
        return Lib::Error::from_code(status);
    }
    return File(m_boot_services, opening_file);
}

Result<void> File::get_info(const GUID *information_type, uint64_t *buffer_size, void *buffer) {
    auto status = m_file->get_info(m_file, information_type, buffer_size, buffer);
    if (status != 0) {
        return Lib::Error::from_code(status);
    }
    return {};
}

Result<void> File::read(uint64_t *buffer_size, void *buffer) {
    auto status = m_file->read(m_file, buffer_size, buffer);
    if (status != 0) {
        return Lib::Error::from_code(status);
    }
    return {};
}

Result<void *> File::read_bytes(uint64_t size) {
    void *bytes;
    TRY(m_boot_services->allocate_pool(EFI::Raw::MemoryType::EFI_LOADER_DATA, size, (void **) &bytes));
    TRY(read(&size, bytes));
    return bytes;
}

Result<void> GraphicsOutputProtocol::query_mode(uint32_t mode_number, size_t *size_of_info, Raw::GraphicsOutputModeInformation **info) {
    auto status = m_graphics_output->query_mode(m_graphics_output, mode_number, size_of_info, info);
    if (status != 0) {
        return Lib::Error::from_code(status);
    }
    return {};
}

Result<Raw::GraphicsOutputModeInformation *> GraphicsOutputProtocol::query_mode(uint32_t mode_number) {
    size_t size;
    Raw::GraphicsOutputModeInformation *mode_information;
    auto status = m_graphics_output->query_mode(m_graphics_output, mode_number, &size, &mode_information);
    if (status != 0) {
        return Lib::Error::from_code(status);
    }
    return mode_information;
}

Result<void> GraphicsOutputProtocol::set_mode(uint32_t mode) {
    auto status = m_graphics_output->set_mode(m_graphics_output, mode);
    if (status != 0) {
        return Lib::Error::from_code(status);
    }
    return {};
}

Result<void> GraphicsOutputProtocol::blit(Raw::BlitPixel *BltBuffer, Raw::BlitOperation BltOperation, uint64_t source_x, uint64_t source_y, uint64_t destination_x, uint64_t destination_y, uint64_t width, uint64_t height, uint64_t delta) {
    auto status = m_graphics_output->blit(m_graphics_output, BltBuffer, BltOperation, source_x, source_y, destination_x, destination_y, width, height, delta);
    if (status != 0) {
        return Lib::Error::from_code(status);
    }
    return {};
}

Result<void> MemoryMap::sanity_check() {
    uint64_t last_address = 0;
    for(int i = 0; i < m_descriptor_count; i++) {
        auto descriptor = m_descriptors[i];
        auto size = descriptor.number_of_pages * 0x1000;
        if(descriptor.physical_start < last_address) {
            // Found overlapping memory
            printf("ERROR!! Overlapping memory found\r\n");
            printf("%d: start = %X, pages = %d, size = %X\r\n", i, descriptor.physical_start, descriptor.number_of_pages, size);
            return Lib::Error::from_code(1);
        }
        last_address = descriptor.physical_start + size;
    }
    return {};
}
