cmake_minimum_required(VERSION 3.16)

project(CustomLibraryTests VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)

find_package(Catch2 REQUIRED)

add_executable(MainTest SDLTests.cpp)
target_link_libraries(MainTest PRIVATE ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${SDL2_TTF_LIBRARY} ${SDL2_MIXER_LIBRARY} CustomLibrary Catch2::Catch2)

include(Catch)
catch_discover_tests(MainTest)