# CMake Build Configuration for SFUmon
cmake_minimum_required(VERSION 3.18)
project(sfumon VERSION 1.0 DESCRIPTION "SFUmon Pokemon-style game" LANGUAGES C)

# Detect if we're cross-compiling
if(CMAKE_CROSSCOMPILING)
    message(STATUS "=== Cross-compiling for ARM64 (BeagleY-AI) ===")
else()
    message(STATUS "=== Native compilation (host) ===")
endif()

# Gets rid of the red squiggles from the .h includes in my .c files
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Compiler options (inherited by sub-folders)
set(CMAKE_C_STANDARD 11)
add_compile_options(-Wall -Werror -Wpedantic -Wextra)
add_compile_options(-fdiagnostics-color)

# Enable address sanitizer for debugging
# (Comment this out to make your code faster or for release builds)
# Note: For cross-compilation, ensure libasan6 is installed on target
# add_compile_options(-fsanitize=address)
# add_link_options(-fsanitize=address)

# Enable PThread library for linking
add_compile_options(-pthread)
add_link_options(-pthread)

# SDL2 Configuration
if(CMAKE_CROSSCOMPILING)
    # Cross-compilation: Use sysroot paths (set by toolchain file)
    if(NOT DEFINED SDL2_INCLUDE_DIRS OR NOT DEFINED SDL2_LIBRARIES)
        message(FATAL_ERROR "SDL2 paths not set by toolchain file!")
    endif()
    message(STATUS "SDL2 Include (cross): ${SDL2_INCLUDE_DIRS}")
    message(STATUS "SDL2 Library (cross): ${SDL2_LIBRARIES}")
    
    # For cross-compilation, manually set SDL2 as an imported target
    add_library(SDL2::SDL2 UNKNOWN IMPORTED)
    set_target_properties(SDL2::SDL2 PROPERTIES
        IMPORTED_LOCATION "${SDL2_LIBRARIES}"
        INTERFACE_INCLUDE_DIRECTORIES "${SDL2_INCLUDE_DIRS}"
    )
else()
    # Native compilation: Find SDL2 on host system
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    message(STATUS "SDL2 Include (native): ${SDL2_INCLUDE_DIRS}")
    message(STATUS "SDL2 Libraries (native): ${SDL2_LIBRARIES}")
    
    # Create imported target for consistency
    if(NOT TARGET SDL2::SDL2)
        add_library(SDL2::SDL2 INTERFACE IMPORTED)
        set_target_properties(SDL2::SDL2 PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${SDL2_INCLUDE_DIRS}"
            INTERFACE_LINK_LIBRARIES "${SDL2_LIBRARIES}"
        )
    endif()
endif()

# Make SDL2 variables available to subdirectories
set(SDL2_INCLUDE_DIRS ${SDL2_INCLUDE_DIRS} CACHE STRING "SDL2 include directories")
set(SDL2_LIBRARIES ${SDL2_LIBRARIES} CACHE STRING "SDL2 libraries")

# SDL2_image Configuration
if(CMAKE_CROSSCOMPILING)
    # Cross-compilation: Manually set paths from sysroot
    set(SDL2_IMAGE_INCLUDE_DIRS "${BEAGLE_SYSROOT}/include/SDL2" CACHE STRING "SDL2_image include dirs")
    set(SDL2_IMAGE_LIBRARIES "${BEAGLE_SYSROOT}/lib/libSDL2_image.so" CACHE STRING "SDL2_image libraries")
    
    # Verify paths exist
    if(NOT EXISTS "${SDL2_IMAGE_LIBRARIES}")
        message(WARNING "SDL2_image library not found at: ${SDL2_IMAGE_LIBRARIES}")
        message(WARNING "You may need to install libsdl2-image-dev on your target sysroot")
    endif()
    
    # Create imported target
    add_library(SDL2_image::SDL2_image UNKNOWN IMPORTED)
    set_target_properties(SDL2_image::SDL2_image PROPERTIES
        IMPORTED_LOCATION "${SDL2_IMAGE_LIBRARIES}"
        INTERFACE_INCLUDE_DIRECTORIES "${SDL2_IMAGE_INCLUDE_DIRS}"
    )
    
    message(STATUS "SDL2_image Include (cross): ${SDL2_IMAGE_INCLUDE_DIRS}")
    message(STATUS "SDL2_image Library (cross): ${SDL2_IMAGE_LIBRARIES}")
else()
    # Native compilation
    find_package(SDL2_image REQUIRED)
    message(STATUS "SDL2_image found (native)")
endif()

# SDL2_mixer Configuration
if(CMAKE_CROSSCOMPILING)
    # Cross-compilation: Manually set paths from sysroot
    set(SDL2_MIXER_LIBRARIES "${BEAGLE_SYSROOT}/lib/libSDL2_mixer.so" CACHE STRING "SDL2_mixer library")
    
    if(NOT EXISTS "${SDL2_MIXER_LIBRARIES}")
        message(WARNING "SDL2_mixer library not found at: ${SDL2_MIXER_LIBRARIES}")
        message(WARNING "You may need to install libsdl2-mixer-dev on your target sysroot")
    endif()
    
    message(STATUS "SDL2_mixer Library (cross): ${SDL2_MIXER_LIBRARIES}")
else()
    # Native compilation
    find_library(SDL2_MIXER_LIBRARIES SDL2_mixer REQUIRED)
    message(STATUS "SDL2_mixer Library (native): ${SDL2_MIXER_LIBRARIES}")
endif()

# SDL2_ttf Configuration
if(CMAKE_CROSSCOMPILING)
    # Cross-compilation: Manually set paths from sysroot
    set(SDL2_TTF_LIBRARIES "${BEAGLE_SYSROOT}/lib/libSDL2_ttf.so" CACHE STRING "SDL2_ttf library")
    
    if(NOT EXISTS "${SDL2_TTF_LIBRARIES}")
        message(WARNING "SDL2_ttf library not found at: ${SDL2_TTF_LIBRARIES}")
        message(WARNING "You may need to install libsdl2-ttf-dev on your target sysroot")
    endif()
    
    message(STATUS "SDL2_ttf Library (cross): ${SDL2_TTF_LIBRARIES}")
else()
    # Native compilation
    find_package(SDL2_ttf REQUIRED)
    message(STATUS "SDL2_ttf found (native)")
endif()

# gpiod Configuration
if(CMAKE_CROSSCOMPILING)
    # Use values provided by toolchain file (FULL PATHS)
    message(STATUS "gpiod Include (cross): ${GPIOD_INCLUDE_DIRS}")
    message(STATUS "gpiod Library (cross): ${GPIOD_LIBRARIES}")
else()
    # Native build: use pkg-config (if available)
    find_package(PkgConfig)
    if(PkgConfig_FOUND)
        pkg_check_modules(GPIOD libgpiod)
        if(GPIOD_FOUND)
            set(GPIOD_INCLUDE_DIRS ${GPIOD_INCLUDE_DIRS})
            set(GPIOD_LIBRARIES ${GPIOD_LIBRARIES})
            message(STATUS "gpiod found (native): ${GPIOD_LIBRARIES}")
        else()
            message(STATUS "gpiod not found on host (simulation mode only)")
        endif()
    else()
        message(STATUS "gpiod not available on host (simulation mode only)")
    endif()
endif()

# Keep for subdirectories
set(GPIOD_INCLUDE_DIRS ${GPIOD_INCLUDE_DIRS} CACHE STRING "gpiod include directories")
set(GPIOD_LIBRARIES ${GPIOD_LIBRARIES} CACHE STRING "gpiod libraries")

# What folders to build
add_subdirectory(hal)
add_subdirectory(app)