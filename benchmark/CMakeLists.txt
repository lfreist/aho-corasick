include_directories(extern)

add_executable(benchmark benchmark.cpp)
target_link_libraries(benchmark PRIVATE nfa nanobench)