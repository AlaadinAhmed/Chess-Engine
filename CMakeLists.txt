cmake_minimum_required(VERSION "3.20")

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(
    FATAL_ERROR
      "In-source builds are not allowed. Please create a build directory and run cmake from there. e.g.:\n  mkdir build\n  cd build\n  cmake .."
  )
endif()

project(OctoKnight)
set(CMAKE_CXX_STANDARD 17)
include_directories(${CMAKE_SOURCE_DIR}/include)
set(CMAKE_CXX_FLAGS_RELEASE
    "${CMAKE_CXX_FLAGS_RELEASE} -march=native -flto -O3 -DNDEBUG")
# set(CMAKE_BUILD_TYPE Release) Only proceed if CMAKE_BUILD_TYPE has NOT been
#
# explicitly set
if(NOT CMAKE_BUILD_TYPE)

  # Get the name of the current build directory
  get_filename_component(BUILD_DIR_NAME ${CMAKE_BINARY_DIR} NAME)

  # Convert to lowercase for case-insensitive matching
  string(TOLOWER ${BUILD_DIR_NAME} BUILD_DIR_NAME_LOWER)

  # Check for "debug" or "release" in the directory name
  if(BUILD_DIR_NAME_LOWER MATCHES "debug")
    set(CMAKE_BUILD_TYPE
        "Debug"
        CACHE STRING "Choose the type of build." FORCE)
    message(
      STATUS
        "Inferred build type: Debug from directory name '${BUILD_DIR_NAME}'")
  elseif(BUILD_DIR_NAME_LOWER MATCHES "release")
    set(CMAKE_BUILD_TYPE
        Release
        CACHE STRING "Choose the type of build." FORCE)
    message(
      STATUS
        "Inferred build type: Release from directory name '${BUILD_DIR_NAME}'")
  endif()

endif()
file(GLOB OCTOKNIGHT_SRC "src/*.cpp")
list(REMOVE_ITEM OCTOKNIGHT_SRC "${CMAKE_SOURCE_DIR}/src/main.cpp")

add_library(OctoKnightLib STATIC ${OCTOKNIGHT_SRC})

add_executable(OctoKnight src/main.cpp)
target_link_libraries(OctoKnight OctoKnightLib pthread)
add_custom_command(TARGET OctoKnight POST_BUILD COMMAND strip $<TARGET_FILE:OctoKnight>)

enable_testing()

add_subdirectory(extern/googletest)

add_executable(OctoKnight_illegal_move_test tests/illegal_move_test.cpp)
target_link_libraries(OctoKnight_illegal_move_test gtest_main OctoKnightLib
                      pthread)
add_test(NAME OctoKnight_illegal_move_test COMMAND OctoKnight_illegal_move_test)

add_executable(debug_movegen tests/debug_movegen.cpp)
target_link_libraries(debug_movegen OctoKnightLib pthread)
