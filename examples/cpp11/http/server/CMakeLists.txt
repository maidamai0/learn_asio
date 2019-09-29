file(GLOB src 
"*.cpp"
"*.hpp")

# add excutable
add_executable(http_server ${src})
target_link_libraries(http_server http_parser)