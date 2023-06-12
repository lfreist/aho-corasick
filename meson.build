project('aho-corasick', 'cpp', default_options : ['cpp_std=c++20'])

ac_include = include_directories('include')
subdir('src')
subdir('test')

executable('main', 'main.cpp', include_directories: ac_include, link_with: nfa)