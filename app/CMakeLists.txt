# Build the SFUmon game app, using the HAL library

# Manually list source files to compile (add more as you implement them)
set(APP_SOURCES
    src/catch.c
    src/collision.c
    #src/game_state.c
    src/game.c
    src/input.c
    src/main.c
    src/map.c
    src/music.c
    src/npc.c
    src/player.c
    src/rendering.c
)

# Create executable
add_executable(sfumon ${APP_SOURCES})

# Make headers available to app source files
target_include_directories(sfumon PRIVATE include)

# Link HAL library (which includes SDL2, SDL2_image, SDL2_mixer, gpiod)
target_link_libraries(sfumon PRIVATE hal pthread)

# Copy executable and assets to final location (only for cross-compilation)
if(CMAKE_CROSSCOMPILING)
    # Copy executable
    add_custom_command(TARGET sfumon POST_BUILD 
        COMMAND "${CMAKE_COMMAND}" -E copy 
            "$<TARGET_FILE:sfumon>"
            "$ENV{HOME}/ensc351/public/sfumon/sfumon" 
        COMMENT "Copying ARM executable to public NFS directory")
    
    # Copy assets folder (PNGs, maps, music)
    add_custom_command(TARGET sfumon POST_BUILD 
        COMMAND "${CMAKE_COMMAND}" -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets" 
            "$ENV{HOME}/ensc351/public/sfumon/assets"
        COMMENT "Copying assets (images, maps, music) to public NFS directory")
else()
    # For native development, copy assets to build directory
    add_custom_command(TARGET sfumon POST_BUILD 
        COMMAND "${CMAKE_COMMAND}" -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets" 
            "$<TARGET_FILE_DIR:sfumon>/assets"
        COMMENT "Copying assets to build directory for local testing")
endif()