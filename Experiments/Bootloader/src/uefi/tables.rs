/*
 * WorkbenchOS
 * Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

use core::ffi::c_void;
use core::fmt::{Debug, Formatter};

use crate::uefi::status::Status;

#[allow(non_camel_case_types)]
type wchar_t = i32;

pub type Handle = *const c_void;

#[repr(C, align(1))]
#[derive(Copy, Clone)]
pub struct GuidParts {
    pub ms1: u32,
    pub ms2: u16,
    pub ms3: u16,
    pub ms4: [u8; 8],
}

#[repr(C, align(1))]
pub union GUID {
    pub fields_8: [u8; 16],
    pub fields_16: [u16; 8],
    pub fields_32: [u32; 4],
    pub fields_64: [u64; 2],
    pub parts: GuidParts,
}

impl Debug for GUID {
    fn fmt(&self, f: &mut Formatter<'_>) -> core::fmt::Result {
        unsafe {
            f.write_fmt(format_args!("{:x}-{:x}-{:x}-{:x}{:x}-{:x}{:x}{:x}{:x}{:x}{:x}",
                                     self.parts.ms1,
                                     self.parts.ms2,
                                     self.parts.ms3,
                                     self.parts.ms4[0],
                                     self.parts.ms4[1],
                                     self.parts.ms4[2],
                                     self.parts.ms4[3],
                                     self.parts.ms4[4],
                                     self.parts.ms4[5],
                                     self.parts.ms4[6],
                                     self.parts.ms4[7])).unwrap();
        }
        Ok(())
    }
}

#[allow(dead_code)]
pub enum MemoryType {
    Reserved = 0,
    LoaderCode = 1,
    LoaderData = 2,
    BootServicesCode = 3,
    BootServicesData = 4,
    RuntimeServicesCode = 5,
    RuntimeServicesData = 6,
    Conventional = 7,
    Unusable = 8,
    AcpiReclaim = 9,
    AcpiNonVolatile = 10,
    MMIO = 11,
    MmioPortSpace = 12,
    PalCode = 13,
    PersistentMemory = 14,
}


#[repr(C, align(4))]
pub(crate) struct TableHeader {
    pub(crate) signature: u64,
    revision: u32,
    header_size: u32,
    crc32: u32,
    reserved: u32,
}

#[allow(dead_code)]
enum ThreadPriorityLevel {
    Application = 4,
    Callback = 8,
    Notify = 16,
    HighLevel = 31,
}

#[repr(C, align(1))]
struct ConfigurationTable {
    vendor_guid: GUID,
    vendor_table: *const c_void,
}

#[repr(C, align(4))]
pub struct PixelBitmask {
    pub red_mask: u32,
    pub green_mask: u32,
    pub blue_mask: u32,
    pub reserved_mask: u32,
}

#[derive(Debug)]
#[allow(dead_code)]
pub enum GraphicsPixelFormat {
    PixelRedGreenBlueReserved8BitPerColor,
    PixelBlueGreenRedReserved8BitPerColor,
    PixelBitMask,
    PixelBltOnly,
    PixelFormatMax,
}

#[repr(C, align(4))]
pub struct GraphicsOutputModeInformation {
    pub version: u32,
    pub width: u32,
    pub height: u32,
    pub pixel_format: GraphicsPixelFormat,
    pub pixel_information: PixelBitmask,
    pub pixels_per_scanline: u32,
}

#[repr(C, align(4))]
pub struct GraphicsOutputProtocolMode {
    pub max_mode: u32,
    pub mode: u32,
    pub info: &'static GraphicsOutputModeInformation,
    pub info_size: usize,
    pub framebuffer_base: usize,
    pub framebuffer_size: usize,
}

/*
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
 */

#[repr(C)]
pub struct GraphicsOutputProtocolTable {
    pub query_mode: unsafe extern "efiapi" fn(this: &GraphicsOutputProtocolTable, mode_number: u32, size_of_info: &usize, interface: &mut *mut GraphicsOutputModeInformation) -> Status,
    pub set_mode: unsafe extern "efiapi" fn(this: &GraphicsOutputProtocolTable, mode_number: u32) -> Status,
    pub blit: unsafe extern "efiapi" fn(),
    pub mode: *const GraphicsOutputProtocolMode,
}

#[repr(C, align(4))]
struct SimpleTextOutputProtocol {
    reset: *const c_void,
    output_string: unsafe extern "efiapi" fn(this: &SimpleTextOutputProtocol, string: &[wchar_t]),
    test_string: *const c_void,
    query_mode: *const c_void,
    set_mode: *const c_void,
    set_attribute: *const c_void,
    set_cursor_position: *const c_void,
    enable_cursor: *const c_void,
    mode: *const c_void,
}

#[repr(C, align(4))]
pub struct SystemTable {
    pub(crate) header: TableHeader,
    firmware_vendor: *const wchar_t,
    firmware_revision: u32,
    console_in_handle: *const c_void,
    con_in: u64,
    console_out_handle: *const c_void,
    console_out: *const SimpleTextOutputProtocol,
    standard_error_handle: *const c_void,
    console_err: *const SimpleTextOutputProtocol,
    runtime_services: *const c_void,
    pub(crate) boot_services: *const BootServicesTable,
    number_of_table_entries: u64,
    configuration_table: *const ConfigurationTable,
}

#[repr(C, align(1))]
pub struct DevicePathProtocol {
    pub kind: u8,
    pub subtype: u8,
    pub length: u16,
}

#[repr(C, align(4))]
pub struct FileInfoTable {
    pub size: u64,
    pub file_size: u64,
    pub physical_size: u64,
    pub create_time: Time,
    pub last_access_time: Time,
    pub modification_time: Time,
    pub attribute: u64,
    pub filename: *const u16,
}

#[repr(C, align(1))]
pub struct Time {
    pub year: u16,
    pub month: u8,
    pub day: u8,
    pub hour: u8,
    pub minute: u8,
    pub second: u8,
    pub pad1: u8,
    pub nanosecond: u32,
    pub time_zone: i16,
    pub daylight: u8,
    pub pad2: u8,
}

#[repr(C, align(4))]
pub struct LoadedImageProtocolTable {
    pub revision: u32,
    pub parent_handle: Handle,
    pub system_table: &'static SystemTable,
    pub device_handle: Handle,
    pub file_path: &'static DevicePathProtocol,
    pub reserved: &'static c_void,
    pub load_options_size: u32,
    pub load_options: &'static c_void,
    pub image_base: &'static c_void,
    pub image_size: u64,
    pub image_code_type: MemoryType,
    pub image_data_type: MemoryType,
    pub unload: unsafe extern "efiapi" fn(handle: Handle) -> Status,
}

#[repr(C, align(4))]
pub struct SimpleFileSystemProtocolTable {
    pub revision: u64,
    pub open_volume: unsafe extern "efiapi" fn(this: &SimpleFileSystemProtocolTable, file: &mut *mut c_void) -> Status,
}

#[repr(C, align(4))]
pub struct FileTable {
    pub(crate) revision: u64,
    pub(crate) open: unsafe extern "efiapi" fn(this: &FileTable, file: &mut *mut c_void, filename: *const u16, mode: u64, attributes: u64) -> Status,
    pub(crate) close: unsafe extern "efiapi" fn(this: &FileTable) -> Status,
    unused1: unsafe extern "efiapi" fn(),
    pub(crate) read: unsafe extern "efiapi" fn(this: &FileTable, buffer_size: &u64, buffer: *mut c_void) -> Status,
    unused2: unsafe extern "efiapi" fn(),
    unused3: unsafe extern "efiapi" fn(),
    unused4: unsafe extern "efiapi" fn(),
    pub(crate) get_info: unsafe extern "efiapi" fn(this: &FileTable, information_type: &GUID, buffer_size: &u64, buffer: *mut c_void) -> Status,
    unused5: unsafe extern "efiapi" fn(),
    unused6: unsafe extern "efiapi" fn(),
    unused7: unsafe extern "efiapi" fn(),
    unused8: unsafe extern "efiapi" fn(),
    unused9: unsafe extern "efiapi" fn(),
    unused10: unsafe extern "efiapi" fn(),
}

#[repr(C, align(4))]
pub struct BootServicesTable {
    header: TableHeader,
    raise_tpl: unsafe extern "efiapi" fn(new_tpl: ThreadPriorityLevel) -> ThreadPriorityLevel,
    restore_tpl: unsafe extern "efiapi" fn(old_tpl: ThreadPriorityLevel),
    allocate_pages: unsafe extern "efiapi" fn() -> Status,
    free_pages: unsafe extern "efiapi" fn() -> Status,
    get_memory_map: unsafe extern "efiapi" fn() -> Status,
    pub(crate) allocate_pool: unsafe extern "efiapi" fn(pool_type: MemoryType, size: u64, buffer: &mut *mut c_void) -> Status,
    free_pool: unsafe extern "efiapi" fn() -> Status,
    create_event: unsafe extern "efiapi" fn() -> Status,
    set_timer: unsafe extern "efiapi" fn() -> Status,
    wait_for_event: unsafe extern "efiapi" fn() -> Status,
    signal_event: unsafe extern "efiapi" fn() -> Status,
    close_event: unsafe extern "efiapi" fn() -> Status,
    check_event: unsafe extern "efiapi" fn() -> Status,
    install_protocol_interface: unsafe extern "efiapi" fn() -> Status,
    reinstall_protocol_interface: unsafe extern "efiapi" fn() -> Status,
    uninstall_protocol_interface: unsafe extern "efiapi" fn() -> Status,
    pub(crate) handle_protocol: unsafe extern "efiapi" fn(handle: Handle, protocol: *const GUID, interface: &mut *mut c_void) -> Status,
    reserved: unsafe extern "efiapi" fn(),
    register_protocol_notify: unsafe extern "efiapi" fn() -> Status,
    locate_handle: unsafe extern "efiapi" fn() -> Status,
    locate_device_path: unsafe extern "efiapi" fn() -> Status,
    install_configuration_table: unsafe extern "efiapi" fn() -> Status,
    load_image: unsafe extern "efiapi" fn() -> Status,
    start_image: unsafe extern "efiapi" fn() -> Status,
    exit: unsafe extern "efiapi" fn() -> Status,
    unload_image: unsafe extern "efiapi" fn() -> Status,
    exit_boot_services: unsafe extern "efiapi" fn() -> Status,
    get_next_monotonic_count: unsafe extern "efiapi" fn() -> Status,
    stall: unsafe extern "efiapi" fn() -> Status,
    pub(crate) set_watchdog_timer: unsafe extern "efiapi" fn(timeout: u64, watchdog_code: u64, data_size: u64, watchdog_data: *const c_void) -> Status,
    connect_controller: unsafe extern "efiapi" fn() -> Status,
    disconnect_controller: unsafe extern "efiapi" fn() -> Status,
    open_protocol: unsafe extern "efiapi" fn() -> Status,
    close_protocol: unsafe extern "efiapi" fn() -> Status,
    open_protocol_information: unsafe extern "efiapi" fn() -> Status,
    protocols_per_handle: unsafe extern "efiapi" fn() -> Status,
    locate_handle_buffer: unsafe extern "efiapi" fn() -> Status,
    pub(crate) locate_protocol: unsafe extern "efiapi" fn(protocol: *const GUID, registration: *const c_void, interface: &mut *mut c_void) -> Status,
    install_multiple_protocol_interfaces: unsafe extern "efiapi" fn() -> Status,
    uninstall_multiple_protocol_interfaces: unsafe extern "efiapi" fn() -> Status,
    calculate_crc32: unsafe extern "efiapi" fn() -> Status,
    create_event_ex: unsafe extern "efiapi" fn() -> Status,
}
