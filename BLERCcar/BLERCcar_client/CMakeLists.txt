cmake_minimum_required(VERSION 3.21)

project(BLERCCar_client VERSION 0.0.1)

# Either compile everytime compiling this project
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/lib/SimpleBLE/simplebluez ${CMAKE_BINARY_DIR}/simplebluez)

# Or add the precompiled to the build.
# Compile once, and link.
message("-- [INFO] Building BLERCCar_client")

add_library(${CMAKE_PROJECT_NAME}_lib BLERCCar_client.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME}_lib PUBLIC # Making this PVIRATE - wont expose all these linkages in subdirectories targets
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/spdlog/build/libspdlog.a
    simplebluez::simplebluez
    pthread
)

target_include_directories(${CMAKE_PROJECT_NAME}_lib PUBLIC # Making this PVIRATE - wont expose all these headers in subdirectories targets
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/spdlog/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../common
)

enable_testing()
add_subdirectory(test)
