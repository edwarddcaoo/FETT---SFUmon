# Build the SFUmon game app, using the HAL library

# Manually list source files to compile (add more as you implement them)
set(APP_SOURCES
    #src/camera.c
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
    src/dialogue.c
)

# ADD THIS — find the SDL2_ttf library
find_package(SDL2_ttf REQUIRED)

# Create executable
add_executable(sfumon ${APP_SOURCES})

# Make headers available to app source files
target_include_directories(sfumon PRIVATE include)

# Link HAL library + SDL2_ttf
target_link_libraries(sfumon PRIVATE hal pthread SDL2_ttf)

# Copy executable and assets to final location (etc...)
if(CMAKE_CROSSCOMPILING)
    add_custom_command(TARGET sfumon POST_BUILD 
        COMMAND "${CMAKE_COMMAND}" -E copy 
            "$<TARGET_FILE:sfumon>"
            "$ENV{HOME}/ensc351/public/sfumon/sfumon" 
        COMMENT "Copying ARM executable to public NFS directory")
    
    add_custom_command(TARGET sfumon POST_BUILD 
        COMMAND "${CMAKE_COMMAND}" -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets" 
            "$ENV{HOME}/ensc351/public/sfumon/assets"
        COMMENT "Copying assets (images, maps, music) to public NFS directory")
else()
    add_custom_command(TARGET sfumon POST_BUILD 
        COMMAND "${CMAKE_COMMAND}" -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets" 
            "$<TARGET_FILE_DIR:sfumon>/assets"
        COMMENT "Copying assets to build directory for local testing")
endif()
