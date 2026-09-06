cmake_minimum_required(VERSION 3.16)

project(
    TetrisComplete
    VERSION 1.0
    LANGUAGES CXX
)

# ============================================================
# C++17
# ============================================================

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================
# Sources
# ============================================================

set(TETRIS_SOURCES
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

add_executable(
    TetrisComplete
    ${TETRIS_SOURCES}
)

target_include_directories(
    TetrisComplete
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ============================================================
# Windows
# GitHub Actions + vcpkg
# ============================================================

if(WIN32)

    message(STATUS "Target: Windows")

    find_package(
        SDL2 CONFIG REQUIRED
    )

    find_package(
        SDL2_ttf CONFIG REQUIRED
    )

    target_link_libraries(
        TetrisComplete
        PRIVATE
        SDL2::SDL2
        SDL2::SDL2main
        SDL2_ttf::SDL2_ttf
    )

    # 日本語文字列をUTF-8としてコンパイル
    if(MSVC)
        target_compile_options(
            TetrisComplete
            PRIVATE
            /utf-8
            /W4
        )
    endif()

# ============================================================
# Linux / Raspberry Pi
# ============================================================

elseif(UNIX)

    message(STATUS "Target: Linux / Raspberry Pi")

    find_package(
        PkgConfig REQUIRED
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

    target_include_directories(
        TetrisComplete
        PRIVATE
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_TTF_INCLUDE_DIRS}
    )

    target_link_directories(
        TetrisComplete
        PRIVATE
        ${SDL2_LIBRARY_DIRS}
        ${SDL2_TTF_LIBRARY_DIRS}
    )

    target_compile_options(
        TetrisComplete
        PRIVATE
        ${SDL2_CFLAGS_OTHER}
        ${SDL2_TTF_CFLAGS_OTHER}
        -Wall
        -Wextra
        -Wpedantic
    )

    target_link_libraries(
        TetrisComplete
        PRIVATE
        ${SDL2_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
        pthread
    )

else()

    message(
        FATAL_ERROR
        "Unsupported platform"
    )

endif()

# ============================================================
# Output directory
# ============================================================

set_target_properties(
    TetrisComplete
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY
    "${CMAKE_BINARY_DIR}/bin"
)

# Visual Studioは通常
# Debug / Releaseフォルダを追加するので、
# 出力先をbuild/binに統一する
if(MSVC)

    set_target_properties(
        TetrisComplete
        PROPERTIES

        RUNTIME_OUTPUT_DIRECTORY_DEBUG
        "${CMAKE_BINARY_DIR}/bin"

        RUNTIME_OUTPUT_DIRECTORY_RELEASE
        "${CMAKE_BINARY_DIR}/bin"

        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO
        "${CMAKE_BINARY_DIR}/bin"

        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL
        "${CMAKE_BINARY_DIR}/bin"
    )

endif()
