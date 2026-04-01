cmake_minimum_required(VERSION 3.10)
project(Inventory LANGUAGES C)

set(SOURCE_FILES
    src/main.c
    src/hashTable.c
    ui/ui.c
)

add_executable(Inventory ${SOURCE_FILES})

target_link_libraries(Inventory m ncurses)
