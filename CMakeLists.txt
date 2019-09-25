cmake_minimum_required(VERSION 3.10)

project(learn_asio)

add_definitions(-DASIO_STANDALONE)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/asio-1.12.2/include)

add_subdirectory(examples)