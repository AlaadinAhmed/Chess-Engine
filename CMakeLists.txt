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
    "${CMAKE_CXX_FLAGS_RELEASE} -march=native -flto -O3 -DNDEBUG -mavx2 -mfma -DUSE_AVX2 -DNNUE_EMBEDDING_OFF")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -mavx2 -mfma -DUSE_AVX2 -DNNUE_EMBEDDING_OFF")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -mfma -DUSE_AVX2 -DNNUE_EMBEDDING_OFF")
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

# Stockfish NNUE probe library (supports HalfKAv2 format from Stockfish 14+)
add_library(nnue_probe STATIC
    extern/stockfish_nnue_probe/bitboard.cpp
    extern/stockfish_nnue_probe/probe.cpp
    extern/stockfish_nnue_probe/evaluate.cpp
    extern/stockfish_nnue_probe/misc.cpp
    extern/stockfish_nnue_probe/position.cpp
    extern/stockfish_nnue_probe/nnue/evaluate_nnue.cpp
    extern/stockfish_nnue_probe/nnue/features/half_ka_v2_hm.cpp
)
target_include_directories(nnue_probe PUBLIC 
    extern/stockfish_nnue_probe
    extern/stockfish_nnue_probe/nnue
)

# Force AVX2 for nnue_probe
target_compile_options(nnue_probe PRIVATE -mavx2 -mfma -DUSE_AVX2)

add_library(OctoKnightLib STATIC ${OCTOKNIGHT_SRC})
target_link_libraries(OctoKnightLib PUBLIC nnue_probe)
# Force AVX2 for OctoKnightLib
target_compile_options(OctoKnightLib PRIVATE -mavx2 -mfma -DUSE_AVX2)

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
