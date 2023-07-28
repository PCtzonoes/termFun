cmake_minimum_required(VERSION 3.26)
project(termFun)

set(CMAKE_CXX_STANDARD 17)

if (APPLE)
    if (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "arm64")
        set(CMAKE_OSX_ARCHITECTURES "arm64")
        set(LDFLAGS "-L/opt/homebrew/opt/ncurses/lib")
        set(CPPFLAGS "-I/opt/homebrew/opt/ncurses/include")
        link_directories(${LDFLAGS})
        include_directories(${CPPFLAGS})
        message(STATUS "Building for ARM64")
    endif ()
endif ()

include_directories(${CMAKE_BINARY_DIR})

find_path(NLOHMANN_JSON_INCLUDE_DIRS "nlohmann/json.hpp")
include_directories(${NLOHMANN_JSON_INCLUDE_DIRS})

file(READ test.json TEST_JSON_CONTENT)
configure_file("${CMAKE_SOURCE_DIR}/template.cpp.in" "${CMAKE_BINARY_DIR}/template.cpp")

add_executable(termFun main.cpp ${CMAKE_BINARY_DIR}/template.cpp)

target_link_libraries(termFun ncurses)