cmake_minimum_required(VERSION 3.16)

project(learn_asio)

# update submodules
add_custom_target(update_dependencies
    COMMAND ${PROXY} git submodule update --init
    COMMAND ${PROXY} git submodule foreach git pull
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
)

# asio definition
add_definitions(-DASIO_STANDALONE -DASIO_NO_DEPRECATED -DASIO_HAS_MOVE)

# generate compile_commands.json if possible
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    link_libraries(pthread)

    # warning flags
    # all warning message specified here will not be allowed in code
    # TODO Add -Wconversion
    set(WARNING "-Wall -Werror -Wpedantic -Wextra")

    # ignore some warnings
    set(WARNING "${WARNING} -Wno-nonnull")

    if(CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 7.0)
        set(WARNING "${WARNING} -Wshadow=local")
        message("g++ version greater than 7.0 use -Wshadow=local")
    endif()

    # other common compile falgs
    set(OTHER_COMPILE_FLAGS "-fexceptions")
endif()

# c++ version
set(CMAKE_CXX_STANDARD 17)

# default c++ compile flags
set(COMMON_COMPILE_FLAGS "${WARNING} ${OTHER_COMPILE_FLAGS}")

# debug compile flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 ${COMMON_COMPILE_FLAGS}")

# release compile flags
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O2 ${COMMON_COMPILE_FLAGS}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(deps/asio/asio/include)

add_library(http_parser deps/http_parser/http_parser.c)
include_directories(deps/http_parser)

add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)
add_subdirectory(deps/spdlog)
include_directories(deps/spdlog/include/)
link_libraries(spdlog)

include_directories(deps/cxxopts/include)
include_directories(deps/magic_enum/include)

add_subdirectory(src)
add_subdirectory(tutorial)
