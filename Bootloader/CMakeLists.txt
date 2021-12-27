project(Bootloader)

SET(LINK_FLAGS -target x86_64-unknown-windows -nostdlib -Wl,-entry:efi_main -Wl,-subsystem:efi_application -fuse-ld=lld-link)

SET(LIBRARY_SOURCES
        ../Library/Convert.cpp
        ../Library/EFI/EfiWrapper.cpp
        ../Library/EFI/EfiWrapper.h
        ../Library/EFI/GUID.h
        ../Library/UnbufferedConsole.cpp
        ../Library/ConsoleIO.cpp
        ../Library/LinearFramebuffer.cpp
        ../Library/LinearFramebuffer.h
        ../Library/ConsoleFont.h
        ../Library/Error.h
        ../Library/Optional.h
        ../Library/Try.h
        ../Library/cstring.cpp
        ../Library/cstring.h
        ../Library/Iterator.h
        ../Library/BootState.h
        ../Library/EFI/MemoryMap.h)
SET(SOURCES main.cpp ${LIBRARY_SOURCES})

add_compile_options(-MTd)
add_compile_options(-target x86_64-unknown-windows)
add_compile_options(-mno-red-zone)
add_compile_options(-fshort-wchar)
add_compile_options(-fno-rtti -ffreestanding -fbuiltin)
add_compile_options(-fno-exceptions)
add_compile_options(-fno-asynchronous-unwind-tables)
add_compile_options(-mno-80387 -mno-mmx -mno-sse -mno-sse2)

add_executable(Bootloader ${SOURCES})

set_target_properties(Bootloader PROPERTIES OUTPUT_NAME BOOTX64.efi)
target_link_options(Bootloader PRIVATE ${LINK_FLAGS})
target_compile_options(Bootloader PRIVATE ${COMPILE_FLAGS})
