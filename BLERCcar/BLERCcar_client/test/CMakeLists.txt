
find_package(GTest REQUIRED)

set(BINARY ${CMAKE_PROJECT_NAME}_test)
message("TestName: " ${BINARY})
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)

set(SOURCES ${TEST_SOURCES})

# message("gal gal" ${SOURCES})
add_executable(${BINARY} ${TEST_SOURCES})

add_test(NAME ${BINARY} COMMAND ${BINARY})

# target_include_directories(${BINARY} PRIVATE
# ${PROJECT_SOURCE_DIR}/lib/spdlog/include
# ${PROJECT_SOURCE_DIR}/include
# ${PROJECT_SOURCE_DIR}/../common
# )
target_link_libraries(${BINARY} PUBLIC
    ${CMAKE_PROJECT_NAME}_lib
    GTest::gtest
    GTest::gtest_main)