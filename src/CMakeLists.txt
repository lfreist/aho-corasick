add_subdirectory(utils)
add_subdirectory(nfa)

add_library(AhoCorasick ahocorasick.cpp)
target_link_libraries(AhoCorasick PRIVATE utils nfa)