cmake_minimum_required(VERSION 3.16)

project(CustomLibraryExamples VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Starfield Starfield/main.cpp)
target_link_libraries(Starfield PRIVATE ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${SDL2_TTF_LIBRARY} ${SDL2_MIXER_LIBRARY} CustomLibrary)

add_executable(Snake Snake/main.cpp)
target_link_libraries(Snake PRIVATE ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${SDL2_TTF_LIBRARY} ${SDL2_MIXER_LIBRARY} CustomLibrary)

add_executable(Logger Logger/main.cpp)
target_link_libraries(Logger PRIVATE CustomLibrary)

add_executable(Equation EquationSolver/main.cpp)
target_link_libraries(Equation PRIVATE CustomLibrary)

add_executable(Parsing Parsing/main.cpp)
target_link_libraries(Parsing PRIVATE CustomLibrary)

# add_executable(VulkanDemo VulkanDemo/main.cpp)
# target_include_directories(VulkanDemo PRIVATE ${VULKAN_INCLUDE_DIRS})
# target_link_libraries(VulkanDemo PRIVATE ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${SDL2_TTF_LIBRARY} ${SDL2_MIXER_LIBRARY} ${VULKAN_LIB_LIST} CustomLibrary)