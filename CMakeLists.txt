cmake_minimum_required(VERSION "3.20")
project(OctoKnight)
set(CMAKE_CXX_STANDARD 17)
include_directories(${CMAKE_SOURCE_DIR}/include)
SET(CMAKE_BUILD_TYPE Release)
file(GLOB OCTOKNIGHT_SRC
     "src/*.cpp"
)
# Check if we are in Debug or Release mode
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")  # Debug flags
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")  # Release flags
endif()

# Other CMake settings
add_executable(OctoKnight ${OCTOKNIGHT_SRC})
