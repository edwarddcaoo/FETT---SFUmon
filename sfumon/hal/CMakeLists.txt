# --- Find SDL2 core ---
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

# SDL2_mixer may not supply a config file, so find manually
find_library(SDL2_MIXER_LIB SDL2_mixer REQUIRED)

# Collect all HAL source files
file(GLOB HAL_SOURCES "src/*.c")

# Create static library
add_library(hal STATIC ${HAL_SOURCES})

# Public include folder (project includes <hal/...>)
target_include_directories(hal PUBLIC ${SDL2_INCLUDE_DIRS})

# Private include folder (HAL includes "header.h")
target_include_directories(hal PRIVATE include/hal)

# Link SDL2, SDL2_image, and SDL2_mixer to HAL
target_link_libraries(hal
    PUBLIC SDL2::SDL2 SDL2_image ${SDL2_MIXER_LIB}
)
