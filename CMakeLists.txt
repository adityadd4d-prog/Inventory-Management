cmake_minimum_required(VERSION 3.10)
project(Inventory LANGUAGES C)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

set(SOURCE_FILES
    src/main.c
    src/hashTable.c
    src/ui.c
)

add_executable(Inventory ${SOURCE_FILES})

target_include_directories(Inventory PRIVATE ${GTK3_INCLUDE_DIRS})
target_link_libraries(Inventory m ${GTK3_LIBRARIES})
