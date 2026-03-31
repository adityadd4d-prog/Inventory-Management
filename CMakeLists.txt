cmake_minimum_required(VERSION 3.10)
project(InventoryManagement C)

set(CMAKE_C_STANDARD 11)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

set(SOURCES
    src/main.c
    src/gui_handlers.c
    src/hashTable.c
)

add_executable(ims ${SOURCES})

target_include_directories(ims PRIVATE
    ${GTK3_INCLUDE_DIRS}
    src/
)

target_link_libraries(ims
    ${GTK3_LIBRARIES}
    m
)

target_compile_options(ims PRIVATE ${GTK3_CFLAGS_OTHER})

# REQUIRED: makes gtk_builder_connect_signals() find your handlers
target_link_options(ims PRIVATE -Wl,--export-dynamic)
