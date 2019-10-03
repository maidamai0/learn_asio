cmake_minimum_required(VERSION 3.10)

project(learn_asio)

# asio definition
add_definitions(-DASIO_STANDALONE -DASIO_NO_DEPRECATED -DASIO_HAS_MOVE)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/asio-1.12.2/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/http_parser)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/fmt_6_0_0/include)

# generate compile_commands.json if possible
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    link_libraries(pthread)
endif()

# warning flags
# all warning message specified here will not be allowed in code
# TODO Add -Wconversion
set(WARNING "-Wall -Werror -Wpedantic -Wextra")

if(CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 7.0)
set(WARNING "${WARNING} -Wshadow=local")
message("g++ version greater than 7.0 use -Wshadow=local")
endif()

# c++ version
set(CMAKE_CXX_STANDARD 17)

# other common compile falgs
set(OTHER_COMPILE_FLAGS "-fexceptions")

# default c++ compile flags
set(COMMON_COMPILE_FLAGS "${WARNING} ${OTHER_COMPILE_FLAGS}")

# debug compile flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 ${COMMON_COMPILE_FLAGS}")

# release compile flags
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O2 ${COMMON_COMPILE_FLAGS}")

# default c++ compile flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG}")

add_subdirectory(fmt_6_0_0)
link_libraries(fmt)

add_subdirectory(http_parser)
add_subdirectory(examples)
add_subdirectory(threads)
add_subdirectory(signal)
add_subdirectory(tutorials)