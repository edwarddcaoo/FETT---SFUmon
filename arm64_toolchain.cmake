# arm64_toolchain.cmake
# Cross-compiling for BeagleBoard ARM64

# Target system
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# Compilers
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)

# Expand HOME directory manually
if(DEFINED ENV{HOME})
    set(HOME_DIR $ENV{HOME})
else()
    message(FATAL_ERROR "HOME environment variable not set")
endif()

# Where to find target libraries and headers
set(BEAGLE_SYSROOT "${HOME_DIR}/beagle-sysroot/beagle-sysroot")
set(CMAKE_FIND_ROOT_PATH ${BEAGLE_SYSROOT})
set(CMAKE_SYSROOT ${BEAGLE_SYSROOT})

# Search behavior
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# --- SDL2 paths for cross-compilation ---
set(SDL2_INCLUDE_DIRS "${BEAGLE_SYSROOT}/include/SDL2" CACHE STRING "SDL2 include dirs" FORCE)
set(SDL2_LIBRARIES "${BEAGLE_SYSROOT}/lib/libSDL2.so" CACHE STRING "SDL2 libraries" FORCE)

# --- libgpiod paths for cross-compilation ---
set(GPIOD_INCLUDE_DIRS "${BEAGLE_SYSROOT}/include" CACHE STRING "gpiod include dirs" FORCE)
set(GPIOD_LIBRARIES    "${BEAGLE_SYSROOT}/lib/libgpiod.so" CACHE STRING "gpiod libraries" FORCE)

# Debug output
message(STATUS "Toolchain: BEAGLE_SYSROOT = ${BEAGLE_SYSROOT}")
message(STATUS "Toolchain: SDL2_INCLUDE_DIRS = ${SDL2_INCLUDE_DIRS}")
message(STATUS "Toolchain: SDL2_LIBRARIES = ${SDL2_LIBRARIES}")
message(STATUS "Toolchain: GPIOD_INCLUDE_DIRS = ${GPIOD_INCLUDE_DIRS}")
message(STATUS "Toolchain: GPIOD_LIBRARIES = ${GPIOD_LIBRARIES}")

# Verify SDL2 paths exist
if(NOT EXISTS "${SDL2_INCLUDE_DIRS}/SDL.h")
    message(FATAL_ERROR "SDL2 headers not found at: ${SDL2_INCLUDE_DIRS}/SDL.h")
endif()
if(NOT EXISTS "${SDL2_LIBRARIES}")
    message(FATAL_ERROR "SDL2 library not found at: ${SDL2_LIBRARIES}")
endif()

message(STATUS "Toolchain: SDL2 paths verified successfully")

# Verify gpiod paths exist
if(NOT EXISTS "${GPIOD_INCLUDE_DIRS}/gpiod.h")
    message(FATAL_ERROR "gpiod headers not found at: ${GPIOD_INCLUDE_DIRS}/gpiod.h")
endif()
if(NOT EXISTS "${GPIOD_LIBRARIES}")
    message(FATAL_ERROR "gpiod library not found at: ${GPIOD_LIBRARIES}")
endif()

message(STATUS "Toolchain: SDL2 and gpiod paths verified successfully")

