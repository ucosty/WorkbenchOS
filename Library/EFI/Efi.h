// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include <EFI/GUID.h>
#include <Types.h>

#define EFICALL __attribute__((ms_abi))

#define EFI_FILE_MODE_READ 0x01

namespace EFI::Raw {
    typedef void *Handle;
    typedef uint64_t Status;
    typedef uint64_t ThreadPriorityLevel;

    struct Time {
        uint16_t year;
        uint8_t month;
        uint8_t day;
        uint8_t hour;
        uint8_t minute;
        uint8_t second;
        uint8_t pad1;
        uint32_t nanosecond;
        int16_t time_zone;
        uint8_t daylight;
        uint8_t pad2;
    };

    struct TableHeader {
        uint64_t signature;
        uint32_t revision;
        uint32_t header_size;
        uint32_t crc32;
        uint32_t reserved;
    };

    struct File {
        uint64_t revision;
        [[nodiscard]] Status (*open)(
            struct File *,
            struct File **,
            const wchar_t *,
            uint64_t,
            uint64_t);
        Status (*close)(struct File *);

        void (*unused1)();

        Status (*read)(struct File *, uint64_t *, void *);

        void (*unused2)();
        void (*unused3)();
        void (*unused4)();

        Status (*get_info)(struct File *,
                           const GUID *information_type,
                           uint64_t *buffer_size,
                           void *buffer);

        void (*unused6)();
        void (*unused7)();
        void (*unused8)();
        void (*unused9)();
        void (*unused10)();
        void (*unused11)();
    };

    struct PixelBitmask {
        uint32_t RedMask;
        uint32_t GreenMask;
        uint32_t BlueMask;
        uint32_t ReservedMask;
    };

    enum GraphicsPixelFormat {
        PixelRedGreenBlueReserved8BitPerColor,
        PixelBlueGreenRedReserved8BitPerColor,
        PixelBitMask,
        PixelBltOnly,
        PixelFormatMax
    };

    struct GraphicsOutputModeInformation {
        uint32_t version;
        uint32_t width;
        uint32_t height;
        GraphicsPixelFormat pixel_format;
        PixelBitmask pixel_information;
        uint32_t pixels_per_scanline;
    };

    struct GraphicsOutputProtocolMode {
        uint32_t max_mode;
        uint32_t mode;
        GraphicsOutputModeInformation *info;
        size_t info_size;
        uint64_t framebuffer_base;
        size_t framebuffer_size;
    };

    struct BlitPixel {
        uint8_t blue;
        uint8_t green;
        uint8_t red;
        uint8_t reserved;
    };

    enum BlitOperation {
        BltVideoFill,
        BltVideoToBltBuffer,
        BltBufferToVideo,
        BltVideoToVideo,
        GraphicsOutputBltOperationMax
    };

    struct GraphicsOutputProtocol {
        Status(EFICALL *query_mode)(GraphicsOutputProtocol *self, uint32_t mode_number, size_t *size_of_info, GraphicsOutputModeInformation **info);
        Status(EFICALL *set_mode)(GraphicsOutputProtocol *self, uint32_t mode);
        Status(EFICALL *blit)(GraphicsOutputProtocol *self, BlitPixel *BltBuffer, BlitOperation BltOperation,
                              uint64_t source_x, uint64_t source_y,
                              uint64_t destination_x, uint64_t destination_y,
                              uint64_t width, uint64_t height,
                              uint64_t delta);
        GraphicsOutputProtocolMode *mode;
    };

    struct SimpleFileSystemProtcol {
        uint64_t revision;
        [[nodiscard]] Status(EFICALL *open_volume)(SimpleFileSystemProtcol *, File **);
    };

    struct SimpleTextOutputProtocol {
        void *Reset;

        Status(EFICALL *output_string)(
            SimpleTextOutputProtocol *self,
            const wchar_t *string);

        void *TestString;
        void *QueryMode;
        void *SetMode;
        void *SetAttribute;

        Status(EFICALL *clear_screen)(
            SimpleTextOutputProtocol *self);

        void *SetCursorPosition;
        void *EnableCursor;
        void *Mode;
    };

    struct FileInfo {
        uint64_t size;
        uint64_t file_size;
        uint64_t physical_size;
        Time create_time;
        Time last_access_time;
        Time modification_time;
        uint64_t attribute;
        const wchar_t *filename;
    };

    enum InterfaceType {
        EFI_NATIVE_INTERFACE,
    };

    enum LocateSearchType {
        EFI_ALL_HANDLES,
        EFI_BY_REGISTER_NOTIFY,
        EFI_BY_PROTOCOL,
    };

    enum AllocateType {
        EFI_ALLOCATE_ANY_PAGES = 0x0,
        EFI_ALLOCATE_MAX_ADDRESS = 0x1,
        EFI_ALLOCATE_ADDRESS = 0x2,
        EFI_ALLOCATE_TYPE_N = 0x3
    };

    enum MemoryType {
        EFI_RESERVED_MEMORY_TYPE,
        EFI_LOADER_CODE,
        EFI_LOADER_DATA,
        EFI_BOOT_SERVICES_CODE,
        EFI_BOOT_SERVICES_DATA,
        EFI_RUNTIME_SERVICES_CODE,
        EFI_RUNTIME_SERVICES_DATA,
        EFI_CONVENTIONAL_MEMORY,
        EFI_UNUSABLE_MEMORY,
        EFI_ACPI_RECLAIM_MEMORY,
        EFI_ACPI_MEMORY_NVS,
        EFI_MEMORY_MAPPED_IO,
        EFI_MEMORY_MAPPED_IO_PORT_SPACE,
        EFI_PAL_CODE,
        EFI_PERSISTENT_MEMORY
    };

    struct MemoryDescriptor {
        uint32_t type;
        uint64_t physical_start;
        uint64_t virtual_start;
        uint64_t number_of_pages;
        uint64_t attribute;
        uint64_t pad;
    };

    enum TimerDelay {
        EFI_TIMER_CANCEL,
        EFI_TIMER_PERIODIC,
        EFI_TIMER_RELATIVE,
    };

    typedef void(EFICALL *EventNotify)(void *event, void *context);

    struct BootServices {
        TableHeader header;

        ThreadPriorityLevel(EFICALL *raise_tpl)(ThreadPriorityLevel new_tpl);

        void(EFICALL *restore_tpl)(ThreadPriorityLevel old_tpl);

        Status(EFICALL *allocate_pages)(
            AllocateType type,
            MemoryType memory_type,
            uint64_t pages,
            uint64_t *memory);

        Status(EFICALL *free_pages)(
            uint64_t memory,
            uint64_t pages);

        Status(EFICALL *get_memory_map)(
            uint64_t *memory_map_size,
            MemoryDescriptor *memory_map,
            uint64_t *map_key,
            uint64_t *descriptor_size,
            uint32_t *descriptor_version);

        [[nodiscard]] Status(EFICALL *allocate_pool)(
            MemoryType pool_type,
            uint64_t size,
            void **buffer);

        Status(EFICALL *free_pool)(
            void *buffer);

        Status(EFICALL *create_event)(
            uint32_t type,
            ThreadPriorityLevel notify_tpl,
            EventNotify notify_function,
            void *notify_context,
            void **event);

        Status(EFICALL *set_timer)(
            void *event,
            TimerDelay type,
            uint64_t trigger_time);

        Status(EFICALL *wait_for_event)(
            uint64_t number_of_events,
            void **event,
            uint64_t *index);

        Status(EFICALL *signal_event)(
            void *event);

        Status(EFICALL *close_event)(
            void *event);

        Status(EFICALL *check_event)(
            void *event);

        Status(EFICALL *install_protocol_interface)(
            Handle handle,
            GUID *protocol,
            InterfaceType interface_type,
            void *interface);

        Status(EFICALL *reinstall_protocol_interface)(
            Handle handle,
            GUID *protocol,
            void *old_interface,
            void *new_interface);

        Status(EFICALL *uninstall_protocol_interface)(
            Handle handle,
            GUID *protocol,
            void *interface);

        [[nodiscard]] Status(EFICALL *handle_protocol)(
            Handle handle,
            const GUID *protocol,
            void **interface);

        void *reserved;

        Status(EFICALL *register_protocol_notify)(
            GUID *protocol,
            void *event,
            void **registration);

        Status(EFICALL *locate_handle)(
            LocateSearchType search_type,
            GUID *protocol,
            void *search_key,
            uint64_t *buffer_size,
            void *buffer);

        Status(EFICALL *locate_device_path)(
            GUID *protocol,
            void **device_path,
            void *device);

        Status(EFICALL *install_configuration_table)(
            GUID *guid,
            void *table);

        Status(EFICALL *load_image)(
            bool boot_policy,
            void *parent_image_handle,
            void *device_path,
            void *source_buffer,
            uint64_t source_size,
            void *image_handle);

        Status(EFICALL *start_image)(
            void *image_handle,
            uint64_t *exit_data_size,
            wchar_t **exit_data);

        Status(EFICALL *exit)(
            void *image_handle,
            Status exit_status,
            uint64_t exit_data_size,
            wchar_t *exit_data);

        Status(EFICALL *unload_image)(
            void *image_handle);

        Status(EFICALL *exit_boot_services)(
            void *image_handle,
            uint64_t map_key);

        Status(EFICALL *get_next_monotonic_count)(
            uint64_t *count);

        Status(EFICALL *stall)(
            uint64_t microseconds);

        Status(EFICALL *set_watchdog_timer)(
            uint64_t timeout,
            uint64_t watchdog_code,
            uint64_t data_size,
            wchar_t *watchdog_data);

        /* 1.1+ */

        Status(EFICALL *connect_controller)(
            void *controller_handle,
            void *driver_image_handle,
            void *remaining_device_path,
            bool recursive);

        Status(EFICALL *disconnect_controller)(
            void *controller_handle,
            void *driver_image_handle,
            void *child_handle);

        Status(EFICALL *open_protocol)(
            Handle handle,
            const GUID *protocol,
            void **interface,
            void *agent_handle,
            void *controller_handle,
            uint32_t attributes);

        Status(EFICALL *close_protocol)(
            Handle handle,
            const GUID *protocol,
            void *agent_handle,
            void *controller_handle);

        Status(EFICALL *open_protocol_information)(
            Handle handle,
            const GUID *protocol,
            void **entry_buffer,
            uint64_t *entry_count);

        Status(EFICALL *protocols_per_handle)(
            Handle handle,
            const GUID *protocol_buffer,
            uint64_t *protocol_buffer_count);

        Status(EFICALL *locate_handle_buffer)(
            LocateSearchType search_type,
            const GUID *protocol,
            void *search_key,
            uint64_t *no_handles,
            void **buffer);

        Status(EFICALL *locate_protocol)(
            const GUID *protocol,
            void *registration,
            void **interface);

        Status(EFICALL *install_multiple_protocol_interfaces)(
            Handle handle,
            ...);

        Status(EFICALL *uninstall_multiple_protocol_interfaces)(
            Handle handle,
            ...);

        Status(EFICALL *calculate_crc32)(
            void *data,
            uint64_t data_size,
            uint32_t *crc32);

        void(EFICALL *copy_mem)(
            void *destination,
            void *source,
            uint64_t length);

        void(EFICALL *set_mem)(
            void *buffer,
            uint64_t size,
            uint8_t value);

        /* 2.0+ */

        Status(EFICALL *create_event_ex)(
            uint32_t type,
            ThreadPriorityLevel notify_tpl,
            EventNotify notify_function,
            void *notify_context,
            const GUID *event_group,
            void **event);
    };

    struct RuntimeServices {
    };

    struct SystemTable {
        TableHeader header;
        wchar_t *firmware_vendor;
        uint32_t FirmwareRevision;
        Handle ConsoleInHandle;
        void *ConIn;
        Handle ConsoleOutHandle;
        SimpleTextOutputProtocol *console_out;
        Handle StandardErrorHandle;
        SimpleTextOutputProtocol *console_err;
        void *RuntimeServices;
        BootServices *boot_services;
        size_t NumberOfTableEntries;
        void *ConfigurationTable;
    };

    struct DevicePathProtocol {
        uint8_t type;
        uint8_t subtype;
        uint8_t length[2];
    };

    struct LoadedImageProtocol {
        uint32_t revision;
        Handle parent_handle;
        SystemTable *system_table;

        Handle device_handle;
        DevicePathProtocol *file_path;
        void *reserved;

        uint32_t load_options_size;
        void *load_options;

        void *image_base;
        uint64_t image_size;
        MemoryType image_code_type;
        MemoryType image_data_type;

        Status(EFICALL *unload)(
            Handle image_handle);
    };
}// namespace EFI::Raw
