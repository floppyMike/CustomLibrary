cmake_minimum_required(VERSION 3.16)

project(CustomLibraryTests VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(MainTest temp.cpp)
target_include_directories(MainTest PRIVATE ../include)
target_link_libraries(MainTest ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${SDL2_TTF_LIBRARY} ${SDL2_MIXER_LIBRARY})