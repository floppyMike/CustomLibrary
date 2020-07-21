cmake_minimum_required(VERSION 3.16)

project(CustomLibrary VERSION 3.0 LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

if (UNIX AND NOT APPLE)
    find_package(SDL2 REQUIRED)
    find_package(SDL2_image REQUIRED)
    find_package(SDL2_ttf REQUIRED)
    find_package(SDL2_mixer REQUIRED)
elseif(WIN32)
	set(SDL2_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/extern/SDL2All/include")

	# Support both 32 and 64 bit builds
	if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
		set(SDL2_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/lib/x64/SDL2.lib;${CMAKE_CURRENT_LIST_DIR}/lib/x64/SDL2main.lib")
	else ()
		set(SDL2_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/extern/SDL2All/lib/x86/SDL2.lib;${CMAKE_CURRENT_LIST_DIR}/extern/SDL2All/lib/x86/SDL2main.lib;${CMAKE_CURRENT_LIST_DIR}/extern/SDL2All/lib/x86/SDL2_image.lib;${CMAKE_CURRENT_LIST_DIR}/extern/SDL2All/lib/x86/SDL2_ttf.lib;${CMAKE_CURRENT_LIST_DIR}/extern/SDL2All/lib/x86/SDL2_mixer.lib")
	endif ()

	string(STRIP "${SDL2_LIBRARY}" SDL2_LIBRARY)
endif()

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS} ${SDL2_MIXER_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(CustomLibrary INTERFACE)
target_include_directories(CustomLibrary INTERFACE ./include)

add_executable(MainTest ./tests/temp.cpp)
target_include_directories(MainTest PRIVATE ./include)
target_link_libraries(MainTest ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${SDL2_TTF_LIBRARY} ${SDL2_MIXER_LIBRARY})
