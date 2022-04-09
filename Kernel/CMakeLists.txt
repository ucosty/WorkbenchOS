project(Kernel)

SET(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld)
SET(SOURCES
        main.cpp
        Exceptions.cpp
        Debugging.cpp
        Memory/MemoryManager.cpp
        Memory/MemoryManager.h
        Memory/Bitmap.cpp
        Memory/Bitmap.h
        CxxAbi.cpp
        Processor.cpp
        Processor.h
        Heap/SlabAllocator.h
        Heap/SlabAllocator.cpp
        Heap/Kmalloc.cpp
        Heap/Kmalloc.h
        ACPI/ACPI.cpp
        ACPI/ACPI.h
        ACPI/DescriptorTables.cpp
        ACPI/DescriptorTables.h
        InterruptVectorTable.cpp
        InterruptVectorTable.h
        APIC.cpp
        APIC.h
        ApInit.S
        Process/Process.h
        Process/Process.cpp
        Process/ProcessManager.h
        Process/ProcessManager.cpp
        Process/StackFrame.h
        Interfaces/PCI.cpp
        Interfaces/PCI.h
        Interfaces/Ports.h
        Memory/Region.cpp
        Memory/Region.h
        Memory/AddressSpace.cpp
        Memory/AddressSpace.h
        Devices/BlockDevice.h
        Devices/MemoryBlockDevice.cpp
        Devices/MemoryBlockDevice.h
        Filesystems/SquashFS.cpp
        Filesystems/SquashFS.h
        Filesystems/VFS.cpp
        Filesystems/VFS.h
        Filesystems/RamdiskFS.cpp
        Filesystems/RamdiskFS.h
        Devices/BlockDeviceReader.cpp
        Devices/BlockDeviceReader.h
        ../Library/ConsoleIO.cpp
        ../Library/LinearFramebuffer.cpp
        ../Library/VirtualAddress.cpp
        )
SET(SOURCES ${SOURCES})

add_compile_options(-target x86_64-pc-none-elf)
add_compile_options(-fno-pie)
add_compile_options(-mcmodel=kernel)
add_compile_options(-mno-red-zone)
add_compile_options(-fshort-wchar)
add_compile_options(-fno-rtti -ffreestanding -fno-builtin -nostdinc++)
add_compile_options(-fno-exceptions)
add_compile_options(-fno-asynchronous-unwind-tables)
add_compile_options(-mno-80387 -mno-mmx -mno-sse -mno-sse2)
add_compile_options(-fdiagnostics-color=always)
add_compile_options(-Wall)
add_compile_options(-Wno-unused-private-field)
add_compile_options(-Wno-unused-variable)
add_compile_options(-fuse-init-array)

add_link_options(-nostdlib)
add_link_options(-fuse-ld=lld)
add_link_options(-no-pie)
add_link_options(-Wl,-T${LINKER_SCRIPT})

include_directories(.)

add_executable(Kernel ${SOURCES})

target_link_libraries(Kernel PRIVATE Std)

set_target_properties(Kernel PROPERTIES OUTPUT_NAME kernel.elf)
