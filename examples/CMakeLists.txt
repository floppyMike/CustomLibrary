cmake_minimum_required(VERSION 3.16)

project(CustomLibraryExamples VERSION 1.0 LANGUAGES CXX)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(THREADS_PREFER_PTHREAD_FLAG ON)

include_directories(${CMAKE_SOURCE_DIR}/external/asio)
link_libraries(Threads::Threads OpenSSL::SSL ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${SDL2_TTF_LIBRARY} ${SDL2_MIXER_LIBRARY} CustomLibrary)

add_executable(Starfield Starfield/main.cpp)
add_executable(Snake Snake/main.cpp)
add_executable(Logger Logger/main.cpp)
add_executable(Pong Pong/main.cpp)
add_executable(Parsing Parsing/main.cpp)
add_executable(Math Math/main.cpp)
add_executable(Graphs Graphs/main.cpp)
add_executable(NeuralNetwork NeuralNetwork/main.cpp)
add_executable(wget wget/main.cpp)
add_executable(genetic GeneticAlgorithm/main.cpp)

# add_executable(VulkanDemo VulkanDemo/main.cpp)
# target_include_directories(VulkanDemo PRIVATE ${VULKAN_INCLUDE_DIRS})
# target_link_libraries(VulkanDemo PRIVATE ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${SDL2_TTF_LIBRARY} ${SDL2_MIXER_LIBRARY} ${VULKAN_LIB_LIST} CustomLibrary)