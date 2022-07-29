/*
 * WorkbenchOS
 * Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#![feature(proc_macro_hygiene)]
#![feature(abi_efiapi)]
#![no_main]
#![no_std]

mod uefi;

#[macro_use]
mod debug;

use utf16_literal::utf16;
use crate::uefi::tables::{BootServicesTable, GuidParts, Handle, SystemTable};
use crate::uefi::boot_services::{BootServices, BootServicesError};
use crate::uefi::file::{FileInfo, Mode};
use crate::uefi::graphics_output_protocol::GraphicsOutputProtocol;
use crate::uefi::loaded_image_protocol::LoadedImageProtocol;
use crate::uefi::simple_file_system_protocol::SimpleFileSystemProtocol;
use crate::uefi::status::Status;

#[derive(Debug)]
#[allow(dead_code)]
enum BootloaderError {
    GraphicsInitialisationError(BootServicesError),
    GenericError(Status),
    BootServicesError(BootServicesError)
}

impl From<Status> for BootloaderError {
    fn from(status: Status) -> Self {
        BootloaderError::GenericError(status)
    }
}

impl From<BootServicesError> for BootloaderError {
    fn from(error: BootServicesError) -> Self {
        BootloaderError::BootServicesError(error)
    }
}

#[no_mangle]
extern "C" fn efi_main(image_handle: Handle, system_table: &SystemTable) -> isize {
    main(image_handle, system_table).unwrap();
    loop {}
}

#[repr(C, align(1))]
struct Elf64Header {
    pub ei_magic: u32,
    pub ei_class: u8,
    pub ei_data: u8,
    pub ei_version: u8,
    pub ei_osabi: u8,
    pub ei_abiversion: u8,
    pub ei_pad: [u8; 7],
    pub e_type: u16,
    pub e_machine: u16,
    pub e_version: u32,
    pub e_entry: u64,
    pub e_phoff: u64,
    pub e_shoff: u64,
    pub e_flags: u32,
    pub e_ehsize: u16,
    pub e_phentsize: u16,
    pub e_phnum: u16,
    pub e_shentsize: u16,
    pub e_shnum: u16,
    pub e_shstrndx: u16,
}

fn main(image_handle: Handle, system_table: &SystemTable) -> Result<(), BootloaderError> {
    let boot_services = BootServices::from_system_table(system_table)?;
    boot_services.disable_watchdog_timer()?;

    // Set a basic graphics mode
    let graphics_handler =  init_graphics(&boot_services)?;
    let graphics_mode = graphics_handler.mode().unwrap();
    debugln!("Set graphics mode: {}x{} {:?}", graphics_mode.info.width, graphics_mode.info.height, graphics_mode.info.pixel_format);

    let loaded_image = boot_services.handle_protocol::<LoadedImageProtocol>(image_handle)?;
    let file_system = boot_services.handle_protocol::<SimpleFileSystemProtocol>(loaded_image.device_handle())?;

    let volume = file_system.open_volume()?;

    // Load the kernel.elf file from disk
    let kernel = volume.open(utf16!("kernel.elf") as *const u16, Mode::Read, Mode::Read)?;
    let kernel_info = kernel.get_info::<FileInfo>(&boot_services)?;
    let kernel_bytes = kernel.read_bytes(&boot_services, kernel_info.size())?;

    // Load the RAMdisk file from disk
    let ramdisk = volume.open(utf16!("ramdisk.bin") as *const u16, Mode::Read, Mode::Read)?;
    let ramdisk_info = ramdisk.get_info::<FileInfo>(&boot_services)?;
    let ramdisk_bytes = ramdisk.read_bytes(&boot_services, ramdisk_info.size())?;

    // Test the Kernel ELF header
    /*
    auto elf_header = static_cast<ELF::Elf64Ehdr *>(kernel_bytes);
    if (elf_header->ei_magic != ELF_MAGIC) {
        panic("Kernel.elf: invalid ELF magic bytes");
    }
    */


    Ok(())
}

#[panic_handler]
fn panic(info: &core::panic::PanicInfo) -> ! {
    debugln!("Panic: {:?}", info);
    loop {}
}

fn init_graphics(boot_services: &BootServices) -> Result<GraphicsOutputProtocol, BootloaderError> {
    let handler = boot_services.locate_protocol::<GraphicsOutputProtocol>()?;
    let mode = handler.mode().unwrap();
    let mut selected_mode = 0;
    for i in 0..mode.max_mode {
        let mode = handler.query_mode(i).unwrap();
        if mode.width == 1280 && mode.height == 1024 {
            selected_mode = i;
        }
    };

    handler.set_mode(selected_mode)?;
    Ok(handler)
}
