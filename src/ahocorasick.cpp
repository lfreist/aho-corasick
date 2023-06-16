/**
 * Copyright 2023, Leon Freist (https://github.com/lfreist)
 * Author: Leon Freist <freist.leon@gmail.com>
 * 
 * This file is part of x-search.
 */

#include <ac/ahocorasick.h>

std::ostream &operator<< (std::ostream &os, Result const &result)
{
        return os << result.match << std::string (" [") << std::to_string (result.start) << std::string (", ")
                  << std::to_string (result.end) << std::string ("]");
}

template<>
AhoCorasick<automaton::NFA>::AhoCorasick (std::vector<std::string> patterns, MatchKind match_kind)
        : _patterns (std::move (patterns)), _match_kind (match_kind), _automaton (_patterns, _match_kind, false)
{}

template<>
std::vector<Result> AhoCorasick<automaton::NFA>::find_all (std::string input)
{
        std::vector<Result> results;
        automaton::State *state = _automaton._start_state;
        automaton::State *prev{nullptr};
        for (size_t index = 0; index < input.size (); ++index)
                {
                        prev = state;
                        state = state->next_state (input[index]);
                        if (state == nullptr)
                                {
                                        // In this case, a failure transition is taken.
                                        // We don't increase the index, since the same input char
                                        // as before is considered again.
                                        state = prev->failed;
                                        --index;
                                        continue;
                                }
                        else if (state == _automaton._dead_state)
                                {
                                        break;
                                }
                        else if (state->is_match ())
                                {
                                        if (_match_kind == MatchKind::STANDARD)
                                                {
                                                        for (auto *match : state->matches)
                                                                {
                                                                        results.push_back({*match, index
                                                                                                      - match->size (), index});
                                                                }
                                                }
                                        else
                                                {
                                                        std::string match = **(state->matches.begin ());
                                                        size_t size = match.size ();
                                                        results.push_back({std::move (match), index - size, index});
                                                }
                                }
                }
        return results;
}