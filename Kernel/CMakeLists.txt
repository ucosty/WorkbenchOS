project(Kernel)

SET(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld)
SET(LIBRARY_SOURCES
        ../Library/cstring.cpp
        ../Library/LinearFramebuffer.cpp
        ../Library/PageStructures.h
        ../Library/GDT.h)
SET(SOURCES main.cpp ${LIBRARY_SOURCES})

add_compile_options(-target x86_64-pc-none-elf)
add_compile_options(-fPIC)
add_compile_options(-mcmodel=kernel)
add_compile_options(-mno-red-zone)
add_compile_options(-fshort-wchar)
add_compile_options(-fno-rtti -ffreestanding -fbuiltin)
add_compile_options(-fno-exceptions)
add_compile_options(-fno-asynchronous-unwind-tables)
add_compile_options(-mno-80387 -mno-mmx -mno-sse -mno-sse2)


add_link_options(-target x86_64-pc-none-elf)
add_link_options(-nostdlib)
add_link_options(-fuse-ld=lld)
add_link_options(-Wl,-T${LINKER_SCRIPT})

add_executable(Kernel ${SOURCES})

set_target_properties(Kernel PROPERTIES OUTPUT_NAME kernel.elf)
#target_link_options(Kernel PRIVATE ${LINK_FLAGS})
#target_compile_options(Kernel PRIVATE ${COMPILE_FLAGS})
