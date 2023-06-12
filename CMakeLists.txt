cmake_minimum_required(VERSION 3.2)
project(x-search CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wno-unused-parameter")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

set(MAIN_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(MAIN_PROJECT ON)
endif ()

# ----------------------------------------------------------------------------------------------------------------------
include_directories(${PROJECT_SOURCE_DIR}/include/)
add_subdirectory(src)
# ----------------------------------------------------------------------------------------------------------------------


IF (${MAIN_PROJECT})
    # ___ Executables __________________________________________________________________________________________________
    add_subdirectory(test)
    add_executable(main main.cpp)
    target_link_libraries(main PRIVATE nfa)
endif ()