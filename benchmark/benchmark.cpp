/**
 * Copyright 2023, Leon Freist (https://github.com/lfreist)
 * Author: Leon Freist <freist.leon@gmail.com>
 * 
 * This file is part of x-search.
 */

#include <nanobench.h>
#include <aho-corasick-cjgdev.hpp>
#include <ac/nfa/nfa.h>

#include <iostream>
#include <cstring>
#include <random>

std::string generate_text(size_t size)
{
        std::string result;
        result.reserve(size);

        std::mt19937 rng(42);  // Set a fixed seed for deterministic output
        std::uniform_int_distribution<char> dist(0x21, 0x7e);

        for (size_t i = 0; i < size; ++i) {
                        result.push_back(dist(rng));
                }

        return result;
}

std::vector<std::string> generate_patterns(int num, size_t min_len, size_t max_len)
{
        std::vector<std::string> patterns(num);
        std::mt19937 rng(9);
        std::uniform_int_distribution<size_t> sizes(min_len, max_len);
        for (auto p : patterns)
                {
                        p.assign (generate_text (sizes(rng)));
                }
        return patterns;
}

size_t ac_nfa (std::string &text, std::vector<std::string> &patterns)
{
        NFA nfa (patterns, MatchKind::STANDARD, false);
        State *state = nfa._start_state;
        size_t result = 0;
        for (auto c : text)
                {
                        if (state->is_match ())
                                {
                                        for (auto i : state->get_matches ())
                                                {
                                                        ++result;
                                                }
                                }
                        state = state->next_state (c);
                        if (state == nfa._fail_state)
                                {
                                        state = nfa._start_state;
                                }
                        if (state == nfa._dead_state)
                                {
                                        std::cout << "\n\ndead\n\n";
                                        break;
                                }
                }
        return result;
}

size_t ac_cjgdev (std::string &text, std::vector<std::string> &patterns)
{
        aho_corasick::trie t;
        for (auto &p : patterns)
                {
                        t.insert (p);
                }
        return t.parse_text (text).size ();
}

size_t naiv (std::string &text, std::vector<std::string> &patterns)
{
        size_t result = 0;

        for (const auto &p : patterns)
                {
                        const char *textPtr = text.c_str ();
                        const char *patternPtr = p.c_str ();

                        while ((textPtr = std::strstr (textPtr, patternPtr)) != nullptr)
                                {
                                        result++;
                                        textPtr += p.length ();
                                }
                }

        return result;
}

int main (int argc, char **argv)
{
        std::cout << "\n-----------------------------------------\n";
        std::cout << "Generating text... ";
        std::string text = generate_text (0xf000000);
        std::cout << "done." << std::endl;
        std::cout << "Generating patterns... ";
        std::vector<std::string> patterns = generate_patterns (10000, 5, 20);
        std::cout << "done." << std::endl;
        std::cout << "-----------------------------------------\n\n";

        ankerl::nanobench::Bench bench;
        bench.title ("Aho-Corasick Comparisons")
                .unit ("byte")
                .batch (text.size ())
                .epochIterations (20)
                .relative (true)
                .warmup (1);

        auto add_benchmark = [&bench]<typename Op> (const std::string &name,
                                                    Op &&op) -> void
        {
          bench.run (name, std::forward<Op> (op));
        };

        add_benchmark ("cjgdev/aho-corasick", [&patterns, &text] ()
        {
          auto res = ac_cjgdev (text, patterns);
          ankerl::nanobench::doNotOptimizeAway (res);
        });
        add_benchmark ("std::strstr", [&patterns, &text] ()
        {
          auto res = naiv (text, patterns);
          ankerl::nanobench::doNotOptimizeAway (res);
        });
        add_benchmark ("lfreist/aho-corasick (NFA)", [&patterns, &text] ()
        {
          auto res = ac_nfa (text, patterns);
          ankerl::nanobench::doNotOptimizeAway (res);
        });

        return 0;
}