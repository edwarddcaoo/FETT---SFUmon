# Build the SFUmon game app, using the HAL library

# Manually list source files to compile (add more as you implement them)
set(APP_SOURCES
    #src/catch.c
    src/collision.c
    #src/game_state.c
    src/game.c
    src/input.c
    src/main.c
    src/map.c
    #src/music.c
    src/npc.c
    src/player.c
    src/rendering.c

)

# Create executable
add_executable(sfumon ${APP_SOURCES})

# Make headers available to app source files
target_include_directories(sfumon PRIVATE include)

# Link HAL library (which includes SDL2)
target_link_libraries(sfumon PRIVATE hal)

# Copy executable to final location
# For BeagleBoard deployment:
if(TARGET_BEAGLE)
    add_custom_command(TARGET sfumon POST_BUILD 
        COMMAND "${CMAKE_COMMAND}" -E copy 
            "$<TARGET_FILE:sfumon>"
            "~/ensc351/work/sfumon/build" 
        COMMENT "Copying ARM executable to public NFS directory")
endif()

# For development, optionally copy assets
# add_custom_command(TARGET sfumon POST_BUILD 
#     COMMAND "${CMAKE_COMMAND}" -E copy_directory
#     "${CMAKE_SOURCE_DIR}/assets" "$<TARGET_FILE_DIR:sfumon>/assets"
#     COMMENT "Copying assets to build directory")