cmake_minimum_required(VERSION 3.10)
project(tud-server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES "./*.cpp" "./*.h")

add_executable(tud-server ${SOURCES})

target_link_libraries(tud-server PRIVATE tud)

install(TARGETS tud-server DESTINATION bin)
