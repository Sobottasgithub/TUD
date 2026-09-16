cmake_minimum_required(VERSION 3.10)
project(tud-peer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES "./*.cpp" "./*.h")

add_executable(tud-peer ${SOURCES})

target_link_libraries(tud-peer PRIVATE tud)
target_link_libraries(tud-peer PRIVATE tablog)

install(TARGETS tud-peer DESTINATION bin)
