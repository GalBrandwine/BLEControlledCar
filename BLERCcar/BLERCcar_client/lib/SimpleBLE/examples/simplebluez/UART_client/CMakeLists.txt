cmake_minimum_required(VERSION 3.21)

project(UART_client)

message("-- [INFO] Building UART_client")
add_executable(uart_client uart_client.cpp)
target_link_libraries(uart_client simplebluez::simplebluez pthread)
