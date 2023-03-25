include_directories(/usr/local/include)
link_directories(/usr/local/lib)

add_executable(client_example client_example.cpp)
target_link_libraries(client_example xfnet pthread)

add_executable(server_example server_example.cpp)
target_link_libraries(server_example xfnet pthread)


