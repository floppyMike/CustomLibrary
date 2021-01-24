cmake_minimum_required(VERSION 3.16)

project(CustomLibrary VERSION 3.0 LANGUAGES CXX 
		DESCRIPTION "A header only library providing a handful of classes and functions useful for c++ projects" 
		HOMEPAGE_URL "https://github.com/floppyMike/CustomLibrary")

# Add option to include and link with libraries

include(GNUInstallDirs)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

# Add include only Library

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)

# Add additional tasks

if (TESTS)
	add_subdirectory("tests")
endif()

if (EXAMPLES)
	add_subdirectory("examples")
endif()