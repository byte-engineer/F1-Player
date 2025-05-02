cmake_minimum_required(VERSION 3.16)

project(MySFMLApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")

add_definitions(-DSFML_STATIC)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

include_directories(${CMAKE_SOURCE_DIR}/vendor/include)

link_directories(${CMAKE_SOURCE_DIR}/vendor/lib)

add_executable(main src/main.cpp)

target_link_libraries(main
    sfml-graphics-s
    sfml-window-s
    sfml-system-s
    winmm
    opengl32
    gdi32
    freetype
)