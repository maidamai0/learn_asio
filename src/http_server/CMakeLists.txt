get_filename_component(_name ${CMAKE_CURRENT_SOURCE_DIR} NAME)
file(GLOB srcs *.cpp)
add_executable(${_name} ${srcs})
target_compile_definitions(${_name} PRIVATE APP_NAME="${_name}")