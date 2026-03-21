cmake_minimum_required(VERSION 3.10)
project(Inventory LANGUAGES C)

# Define a variable to hold all source files
set(SOURCE_FILES
    Inventory.c 
    hashTable.c 
    cosmetic.c
)

# Add an executable target using the source files
add_executable(Inventory ${SOURCE_FILES})

# Linking The Math Library as it is not standard
target_link_libraries(Inventory m)
