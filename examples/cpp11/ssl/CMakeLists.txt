# add excutable
add_executable(ssl_server server.cpp)
add_executable(ssl_client client.cpp)

target_link_libraries(ssl_server ssl crypto)
target_link_libraries(ssl_client ssl crypto)