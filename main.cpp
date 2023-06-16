#include <ac/ahocorasick.h>
#include <iostream>

int main (int argc, char **argv)
{
        std::vector<std::string> patterns {"tern", "er"};
        AhoCorasick<automaton::NFA> searcher(std::move(patterns), MatchKind::LEFTMOST_FIRST);
        auto res = searcher.find_all ("pattern!");
        for (const auto& r : res)
                {
                        std::cout << r << std::endl;
                }
        return 0;
}