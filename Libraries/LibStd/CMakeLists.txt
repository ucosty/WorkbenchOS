
SET(SOURCES
        Convert.cpp
        CString.cpp
        Stack.cpp
        String.cpp
        StringView.cpp
        StringSplitter.cpp
        Path.cpp
        Format.cpp
        )

enable_testing()

add_compile_options(-mno-red-zone)
add_compile_options(-fshort-wchar)
add_compile_options(-fno-rtti -ffreestanding -fno-builtin -nostdinc++)
add_compile_options(-fno-exceptions)
add_compile_options(-fno-asynchronous-unwind-tables)
add_compile_options(-fno-stack-protector)
add_compile_options(-mno-80387 -mno-mmx -mno-sse -mno-sse2)
add_compile_options(-fdiagnostics-color=always)
add_compile_options(-Wall)
add_compile_options(-Wno-unused-private-field)
add_compile_options(-Wno-unused-variable)
add_compile_options(-fuse-init-array)

add_library(Std STATIC ${SOURCES})

add_subdirectory(tests)
