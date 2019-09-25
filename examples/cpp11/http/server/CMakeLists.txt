file(GLOB src 
"*.cpp"
"*.hpp")

# add excutable
add_executable(http_server ${src})