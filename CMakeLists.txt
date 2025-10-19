cmake_minimum_required(VERSION "3.20")
project(OctoKnight)
set(CMAKE_CXX_STANDARD 17)
include_directories(${CMAKE_SOURCE_DIR}/include)
SET(CMAKE_BUILD_TYPE Release)

file(GLOB OCTOKNIGHT_SRC
     "src/*.cpp"
)
list(REMOVE_ITEM OCTOKNIGHT_SRC "${CMAKE_SOURCE_DIR}/src/main.cpp")

add_library(OctoKnightLib STATIC ${OCTOKNIGHT_SRC})

add_executable(OctoKnight src/main.cpp)
target_link_libraries(OctoKnight OctoKnightLib pthread)

enable_testing()

add_subdirectory(extern/googletest)

add_executable(OctoKnight_illegal_move_test tests/illegal_move_test.cpp)
target_link_libraries(
    OctoKnight_illegal_move_test
    gtest_main
    OctoKnightLib
    pthread
)
add_test(NAME OctoKnight_illegal_move_test COMMAND OctoKnight_illegal_move_test)