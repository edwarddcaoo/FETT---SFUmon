# CMakeList.txt for HAL (Hardware Abstraction Layer)

# Find SDL2_image and SDL2_mixer
find_package(SDL2_image REQUIRED)

# Find SDL_mixer manually if needed
find_library(SDL2_MIXER_LIB SDL2_mixer REQUIRED)

# Collect all HAL source files
file(GLOB HAL_SOURCES "src/*.c")

# Create static library
add_library(hal STATIC ${HAL_SOURCES})

# Make headers publicly available to OTHER modules (with hal/ prefix)
target_include_directories(hal PUBLIC include)

# Make headers available to HAL's own source files (without hal/ prefix)
target_include_directories(hal PRIVATE include/hal)

# Link SDL2, SDL2_image, and SDL2_mixer to HAL
target_link_libraries(hal PUBLIC SDL2::SDL2 SDL2_image ${SDL2_MIXER_LIB})