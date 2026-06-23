cmake_minimum_required(VERSION 3.10)

project(CPP_TETRIS VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

include(FetchContent)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
)
FetchContent_MakeAvailable(raylib)


include_directories(
    ${PROJECT_SOURCE_DIR}/include
)

add_executable(CPP_TETRIS src/main.cpp
src/game.cpp)

target_link_libraries(CPP_TETRIS PRIVATE raylib)

set_target_properties(CPP_TETRIS PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
)
