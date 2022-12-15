// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include <EFI/Efi.h>
#include <EFI/MemoryMap.h>
#include <EFI/Error.h>
#include <EFI/Error.h>
#include <LibStd/Try.h>

namespace EFI {
const char *status_code(Raw::Status status);
const char *memory_type(MemoryType type);
struct MemoryMap;

constexpr static const GUID acpi_root_table_guid = GUID{
    .ms1 = 0x8868e871,
    .ms2 = 0xe4f1,
    .ms3 = 0x11d3,
    .ms4 = {0xbc, 0x22, 0x00, 0x80, 0xc7, 0x3c, 0x88, 0x81}};

class BootServices {
public:
    explicit BootServices(Raw::BootServices *boot_services) : m_boot_services(boot_services) {}

    Std::Result<uint64_t, EFI::Error> allocate_pages(Raw::AllocateType type, MemoryType memory_type, uint64_t pages);
    Std::Result<void, EFI::Error> free_pages(uint64_t memory, uint64_t pages);
    Std::Result<void, EFI::Error> allocate_pool(MemoryType pool_type, uint64_t size, void **buffer);
    Std::Result<void, EFI::Error> free_pool(void *buffer);
    Std::Result<void, EFI::Error> exit_boot_services(void *image_handle, uint64_t map_key);

    template<typename T>
    Std::Result<T, EFI::Error> handle_protocol(Raw::Handle handle) {
        void *interface;
        auto status = m_boot_services->handle_protocol(handle, T::guid(), (void **) &interface);
        if (status != 0) {
            return Error(status);
        }
        return Std::Result<T, EFI::Error>::of(T(this, interface));
    }
    Std::Result<void, EFI::Error> locate_protocol(const EFI::GUID *protocol, void *registration, void **interface);
    template<typename T>
    Std::Result<T, EFI::Error> locate_protocol() {
        void *protocol = nullptr;
        TRY(locate_protocol(T::guid(), nullptr, (void **) &protocol));
        return T(protocol);
    }
    Std::Result<void, EFI::Error> set_watchdog_timer(uint64_t timeout, uint64_t watchdog_code, uint64_t data_size, wchar_t *watchdog_data);
    Std::Result<void, EFI::Error> get_memory_map(uint64_t *memory_map_size, MemoryDescriptor *memory_map, uint64_t *map_key, uint64_t *descriptor_size, uint32_t *descriptor_version);
    Std::Result<MemoryMap, EFI::Error> get_memory_map();

private:
    Raw::BootServices *m_boot_services;
};

class GraphicsOutputProtocol {
public:
    explicit GraphicsOutputProtocol(void *protocol) : m_graphics_output((Raw::GraphicsOutputProtocol *) protocol) {}
    static const GUID *guid() { return &s_guid; }
    Std::Result<void, EFI::Error> query_mode(uint32_t mode_number, size_t *size_of_info, Raw::GraphicsOutputModeInformation **info);
    Std::Result<Raw::GraphicsOutputModeInformation *, EFI::Error> query_mode(uint32_t mode_number);
    Std::Result<void, EFI::Error> set_mode(uint32_t mode);
    Std::Result<void, EFI::Error> blit(Raw::BlitPixel *BltBuffer, Raw::BlitOperation BltOperation, uint64_t source_x, uint64_t source_y, uint64_t destination_x, uint64_t destination_y, uint64_t width, uint64_t height, uint64_t delta);
    [[nodiscard]] Raw::GraphicsOutputProtocolMode *mode() const { return m_graphics_output->mode; }

private:
    Raw::GraphicsOutputProtocol *m_graphics_output;
    constexpr static const GUID s_guid = GUID{
        .ms1 = 0x9042a9de,
        .ms2 = 0x23dc,
        .ms3 = 0x4a38,
        .ms4 = {0x96, 0xfb, 0x7a, 0xde, 0xd0, 0x80, 0x51, 0x6a}};
};

class FileInfo {
public:
    explicit FileInfo(void *buffer) : m_file_info((Raw::FileInfo *) buffer) {}
    static const GUID *guid() { return &s_guid; }
    [[nodiscard]] uint64_t size() const { return m_file_info->size; }
    [[nodiscard]] uint64_t file_size() const { return m_file_info->file_size; }
    [[nodiscard]] uint64_t physical_size() const { return m_file_info->physical_size; }
    [[nodiscard]] uint64_t attribute() const { return m_file_info->attribute; }
    [[nodiscard]] const wchar_t *filename() const { return m_file_info->filename; }

private:
    Raw::FileInfo *m_file_info;
    constexpr static const GUID s_guid = GUID{
        .ms1 = 0x09576e92,
        .ms2 = 0x6d3f,
        .ms3 = 0x11d2,
        .ms4 = {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}};
};

class File {
public:
    explicit File(BootServices *boot_services, Raw::File *file) : m_boot_services(boot_services), m_file(file) {}
    Std::Result<File, EFI::Error> open(const wchar_t *filename, uint64_t mode, uint64_t attributes);
    Std::Result<void, EFI::Error> get_info(const GUID *information_type, uint64_t *buffer_size, void *buffer);
    template<typename T>
    Std::Result<T, EFI::Error> get_info() {
        auto guid = T::guid();
        uint64_t buffer_size;
        void *buffer;
        TRY(get_info(guid, &buffer_size, nullptr));
        TRY(m_boot_services->allocate_pool(MemoryType::EFI_LOADER_DATA, buffer_size, (void **) &buffer));
        TRY(get_info(guid, &buffer_size, (void **) buffer));
        return T(buffer);
    }
    Std::Result<void, EFI::Error> read(uint64_t *buffer_size, void *buffer);
    Std::Result<void *, EFI::Error> read_bytes(uint64_t size);

private:
    Raw::File *m_file;
    BootServices *m_boot_services;
};

class LoadedImageProtocol {
public:
    LoadedImageProtocol(BootServices *boot_services, void *loaded_image) : m_boot_services(boot_services), m_loaded_image((Raw::LoadedImageProtocol *) loaded_image) {}
    static const GUID *guid() { return &s_guid; }
    [[nodiscard]] Raw::Handle device_handle() const { return m_loaded_image->device_handle; }

private:
    Raw::LoadedImageProtocol *m_loaded_image;
    BootServices *m_boot_services;
    constexpr static const GUID s_guid = GUID{
        .ms1 = 0x5B1B31A1,
        .ms2 = 0x9562,
        .ms3 = 0x11d2,
        .ms4 = {0x8E, 0x3F, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B}};
};

class SimpleFileSystemProtcol {
public:
    SimpleFileSystemProtcol(BootServices *boot_services, void *filesystem) : m_boot_services(boot_services), m_filesystem((Raw::SimpleFileSystemProtcol *) filesystem) {}
    Std::Result<File, EFI::Error> open_volume();
    static const GUID *guid() { return &s_guid; }

private:
    Raw::SimpleFileSystemProtcol *m_filesystem;
    BootServices *m_boot_services;
    constexpr static const GUID s_guid = GUID{
        .ms1 = 0x964e5b22,
        .ms2 = 0x6459,
        .ms3 = 0x11d2,
        .ms4 = {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}};
};
}// namespace EFI