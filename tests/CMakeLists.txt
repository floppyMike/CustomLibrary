cmake_minimum_required(VERSION 3.16)

project(CustomLibraryTests VERSION 2.0 LANGUAGES CXX)
enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)

include(GNUInstallDirs)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)

find_package(Threads REQUIRED)
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
link_libraries(Threads::Threads ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${SDL2_TTF_LIBRARY} ${SDL2_MIXER_LIBRARY} CustomLibrary ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES})

add_executable(Tests Tests.cpp)
add_executable(Features FeatureTests.cpp)