/*
 * WorkbenchOS
 * Copyright (c) 2023 Matthew Costa <ucosty@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#[repr(C, align(1))]
#[derive(Copy, Clone)]
pub struct MemoryDescriptor {
    pub kind: u32,
    pub physical_start: u64,
    pub virtual_start: u64,
    pub number_of_pages: u64,
    pub attribute: u64,
    pub pad: u64,
}

#[repr(C, align(1))]
#[derive(Copy, Clone)]
pub struct MemoryMap {
    pub descriptors: &'static MemoryDescriptor,
    pub size: usize,
    pub descriptor_count: u64,
    pub key: u64,
    pub descriptor_size: u64,
    pub descriptor_version: u32,
}

#[repr(C, align(1))]
#[derive(Copy, Clone)]
pub struct Framebuffer {
    pub size: u64,
    pub width: u64,
    pub height: u64,
}

#[repr(C, align(1))]
#[derive(Copy, Clone)]
pub struct VirtualMapping {
    pub physical_base: u64,
    pub virtual_base: u64,
    pub size: u64,
}

#[repr(C, align(1))]
#[derive(Copy, Clone)]
pub struct KernelAddressSpace {
    pub kernel_page_directory_virtual_address: u64,
    pub base: u64,
    pub size: u64,

    pub kernel: VirtualMapping,
    pub boot_state: VirtualMapping,
    pub initial_pages: VirtualMapping,
    pub memory_map: VirtualMapping,
    pub framebuffer: VirtualMapping,
    pub stack: VirtualMapping,
    pub frame_allocator: VirtualMapping,
}

#[repr(C, align(1))]
#[derive(Copy, Clone)]
pub struct Ramdisk {
    pub address: u64,
    pub size: u64,
}

#[repr(C, align(1))]
#[derive(Copy, Clone)]
pub struct BootState {
    pub physical_memory_size: u64,
    pub acpi_root_table_address: u64,
    pub framebuffer: Framebuffer,
    pub memory_map: MemoryMap,
    pub kernel_address_space: KernelAddressSpace,
    pub ramdisk: Ramdisk,
}
