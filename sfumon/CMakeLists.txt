# CMake Build Configuration for SFUmon
cmake_minimum_required(VERSION 3.18)
project(sfumon VERSION 1.0 DESCRIPTION "SFUmon Pokemon-style game" LANGUAGES C)

# Gets rid of the red squiggles from the .h includes in my .c files
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Compiler options (inherited by sub-folders)
set(CMAKE_C_STANDARD 11)
add_compile_options(-Wall -Werror -Wpedantic -Wextra)
add_compile_options(-fdiagnostics-color)

# Enable address sanitizer for debugging
# (Comment this out to make your code faster or for release builds)
add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

# Enable PThread library for linking
add_compile_options(-pthread)
add_link_options(-pthread)

# Option for target platform
option(TARGET_BEAGLE "Build for BeagleBoard with hardware" OFF)

if(TARGET_BEAGLE)
    add_definitions(-DTARGET_BEAGLE)
    message(STATUS "Building for BeagleBoard (hardware mode)")
else()
    message(STATUS "Building for host (simulation mode)")
endif()

# Find SDL2 packages (needed by both HAL and APP)
find_package(SDL2 REQUIRED)

# What folders to build
add_subdirectory(hal)
add_subdirectory(app)