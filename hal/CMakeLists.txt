# CMakeList.txt for HAL (Hardware Abstraction Layer)

# Collect all HAL source files
# Explicitly list all HAL source files
set(HAL_SOURCES
    src/audio.c
    src/button.c
    src/display.c
    src/joystick.c
    src/sprite.c
    src/storage.c
)

# Create static library
add_library(hal STATIC ${HAL_SOURCES})

# Make headers publicly available to OTHER modules (with hal/ prefix)
target_include_directories(hal PUBLIC include)

# Make headers available to HAL's own source files (without hal/ prefix)
target_include_directories(hal PRIVATE include/hal)

# Link SDL2, SDL2_image, and SDL2_mixer to HAL
target_link_libraries(hal PUBLIC 
    SDL2::SDL2 
    SDL2_image::SDL2_image
    ${SDL2_MIXER_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
)

# Tell the linker where to search for libraries
if(CMAKE_CROSSCOMPILING)
    target_link_directories(hal PUBLIC ${BEAGLE_SYSROOT}/lib/pulseaudio)
endif()

# Add gpiod support if cross-compiling
if(CMAKE_CROSSCOMPILING)
    target_include_directories(hal PUBLIC ${GPIOD_INCLUDE_DIRS})
    target_link_libraries(hal PUBLIC ${GPIOD_LIBRARIES})
    message(STATUS "HAL: Including gpiod support for hardware GPIO")
endif()

# Add math library (needed for some operations)
target_link_libraries(hal PUBLIC m)