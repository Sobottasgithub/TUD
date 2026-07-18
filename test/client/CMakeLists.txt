cmake_minimum_required(VERSION 3.10)
project(tud-client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES "./*.cpp" "./*.h")

add_executable(tud-client ${SOURCES})

target_link_libraries(tud-client PRIVATE tud)
target_link_libraries(tud-client PRIVATE tablog)

install(TARGETS tud-client DESTINATION bin)
