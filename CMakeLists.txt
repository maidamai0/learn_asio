cmake_minimum_required(VERSION 3.10)

project(learn_asio)

add_definitions(-DASIO_STANDALONE)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/asio-1.12.2/include)

set(CMAKE_CXX_STANDARD 17)

# generate compile_commands.json if possible
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    link_libraries(pthread)
endif()


add_subdirectory(examples)