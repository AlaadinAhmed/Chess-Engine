cmake_minimum_required(VERSION "3.20")
project(OctoKnight)
set(CMAKE_CXX_STANDARD 17)
include_directories(${CMAKE_SOURCE_DIR}/include)
SET(CMAKE_BUILD_TYPE Release)

file(GLOB OCTOKNIGHT_SRC
     "src/*.cpp"
)

add_library(OctoKnightLib STATIC ${OCTOKNIGHT_SRC})

add_executable(OctoKnight src/main.cpp)
target_link_libraries(OctoKnight OctoKnightLib)

enable_testing()

add_subdirectory(extern/googletest)

file(GLOB TEST_SRC
     "tests/*.cpp"
)

add_executable(OctoKnight_test ${TEST_SRC})

target_link_libraries(
    OctoKnight_test
    gtest_main
    OctoKnightLib
)