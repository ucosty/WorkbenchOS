
SET(SOURCES
        Test.cpp
        )

add_compile_options(-fshort-wchar)
add_compile_options(-fno-rtti -ffreestanding -fno-builtin -nostdinc++)
add_compile_options(-fno-exceptions)
add_compile_options(-fno-asynchronous-unwind-tables)
add_compile_options(-fno-stack-protector)

add_library(Testable STATIC ${SOURCES})
target_link_libraries(Testable PRIVATE Std)
