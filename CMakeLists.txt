cmake_minimum_required(VERSION 3.16)

project(
    TetrisComplete
    VERSION 1.0
    LANGUAGES CXX
)

# ============================================================
# C++17
# ============================================================

set(
    CMAKE_CXX_STANDARD
    17
)

set(
    CMAKE_CXX_STANDARD_REQUIRED
    ON
)

set(
    CMAKE_CXX_EXTENSIONS
    OFF
)

# ============================================================
# SDL2
# ============================================================

find_package(
    PkgConfig
    REQUIRED
)

pkg_check_modules(
    SDL2
    REQUIRED
    sdl2
)

pkg_check_modules(
    SDL2_TTF
    REQUIRED
    SDL2_ttf
)

# ============================================================
# Executable
# ============================================================

add_executable(
    TetrisComplete

    src/main.cpp

    src/settings.cpp

    src/tetromino.cpp

    src/game.cpp

    src/input.cpp

    src/progression.cpp

    src/stages.cpp

    src/save.cpp

    src/ui.cpp

    src/renderer.cpp

    src/renderer3d.cpp
)

# ============================================================
# Include directories
# ============================================================

target_include_directories(
    TetrisComplete
    PRIVATE

    ${SDL2_INCLUDE_DIRS}

    ${SDL2_TTF_INCLUDE_DIRS}

    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ============================================================
# Library directories
# ============================================================

target_link_directories(
    TetrisComplete
    PRIVATE

    ${SDL2_LIBRARY_DIRS}

    ${SDL2_TTF_LIBRARY_DIRS}
)

# ============================================================
# Compile flags
# ============================================================

target_compile_options(
    TetrisComplete
    PRIVATE

    ${SDL2_CFLAGS_OTHER}

    ${SDL2_TTF_CFLAGS_OTHER}

    -Wall
    -Wextra
    -Wpedantic
)

# ============================================================
# Link
# ============================================================

target_link_libraries(
    TetrisComplete
    PRIVATE

    ${SDL2_LIBRARIES}

    ${SDL2_TTF_LIBRARIES}
)

# ============================================================
# Linux / Raspberry Pi
# ============================================================

if(UNIX AND NOT APPLE)
    target_link_libraries(
        TetrisComplete
        PRIVATE
        pthread
    )
endif()

# ============================================================
# Output
# ============================================================

set_target_properties(
    TetrisComplete
    PROPERTIES

    RUNTIME_OUTPUT_DIRECTORY
    "${CMAKE_BINARY_DIR}/bin"
)
