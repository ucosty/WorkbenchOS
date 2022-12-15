
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

add_compile_options(-fshort-wchar)
add_compile_options(-fno-rtti -ffreestanding -fno-builtin -nostdinc++)
add_compile_options(-fno-exceptions)
add_compile_options(-fno-asynchronous-unwind-tables)
add_compile_options(-fno-stack-protector)

add_library(Std STATIC ${SOURCES})

add_subdirectory(tests)