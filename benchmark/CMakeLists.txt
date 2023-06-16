configure_file(files/harry_potter_1.txt files/harry_potter_1.txt COPYONLY)

include_directories(extern)

add_executable(benchmark benchmark.cpp)
target_link_libraries(benchmark PRIVATE AhoCorasick nfa nanobench)