cmake_minimum_required(VERSION "3.20")

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(
    FATAL_ERROR
      "In-source builds are not allowed. Please create a build directory and run cmake from there. e.g.:\n  mkdir build\n  cd build\n  cmake .."
  )
endif()

project(OctoKnight)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_CXX_STANDARD 17)
include_directories(${CMAKE_SOURCE_DIR}/include)
set(CMAKE_CXX_FLAGS_RELEASE
    "${CMAKE_CXX_FLAGS_RELEASE} -march=native -flto -O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
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
        Debug
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

# NNUE probe library
add_library(nnue_probe STATIC
    extern/nnue-probe/src/nnue.cpp
    extern/nnue-probe/src/misc.cpp
)
target_include_directories(nnue_probe PUBLIC extern/nnue-probe/src)
# Enable SIMD optimizations if available
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-mavx2 HAS_AVX2)
if(HAS_AVX2)
    target_compile_options(nnue_probe PRIVATE -mavx2)
endif()

add_library(OctoKnightLib STATIC ${OCTOKNIGHT_SRC})
target_link_libraries(OctoKnightLib PUBLIC nnue_probe)

add_executable(OctoKnight src/main.cpp)
target_link_libraries(OctoKnight OctoKnightLib pthread)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
  add_custom_command(
    TARGET OctoKnight
    POST_BUILD
    COMMAND strip $<TARGET_FILE:OctoKnight>)
endif()

enable_testing()

add_subdirectory(extern/googletest)

file(GLOB TEST_FILES "tests/*.cpp")

foreach(TEST_FILE ${TEST_FILES})
  get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
  add_executable(${TEST_NAME} ${TEST_FILE})
  target_link_libraries(${TEST_NAME} gtest_main OctoKnightLib pthread)
  add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()
