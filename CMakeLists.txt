cmake_minimum_required(VERSION "3.20")
project(OctoKnight)
set(CMAKE_CXX_STANDARD 17)
include_directories(${CMAKE_SOURCE_DIR}/include)
file(GLOB OCTOKNIGHT_SRC
     "src/*.cpp"
)

add_executable(OctoKnight ${OCTOKNIGHT_SRC})