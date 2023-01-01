cmake_minimum_required(VERSION 3.21)

project(BLERCCar_client VERSION 0.0.1)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/lib/SimpleBLE/simplebluez ${CMAKE_BINARY_DIR}/simplebluez)

message("-- [INFO] Building BLERCCar_client")
add_executable(blerccar_client BLERCCar_client.cpp)
target_link_libraries(blerccar_client simplebluez::simplebluez pthread)
