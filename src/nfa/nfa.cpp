/**
 * Copyright 2023, Leon Freist (https://github.com/lfreist)
 * Author: Leon Freist <freist.leon@gmail.com>
 * 
 * This file is part of builddir.
 */

#include <ac/search.h>
#include <ac/nfa/nfa.h>

#include <queue>
#include <set>

namespace automaton {

bool State::is_match () const
{
        return !matches.empty ();
}

State *State::next_state (CodePoint code_point) const
{
        return transitions[code_point];
}


// ===== NFA ===========================================================================================================

NFA::NFA (const std::vector<std::string> &patterns, MatchKind match_kind, bool ascii_i_case)
        : _match_kind (match_kind), _ignore_case (ascii_i_case)
{
        init_start_state ();
        build_trie (patterns);
        add_start_state_loop ();
        add_dead_state_loop ();
        add_failure_transitions ();
        close_start_state_loop_for_leftmost ();
}

NFA::~NFA ()
{
        delete _dead_state;
        delete _start_state;
        for (auto *s : _states)
                {
                        delete s;
                }
}

void NFA::build_trie (const std::vector<std::string> &patterns)
{
        for (const auto &pattern : patterns)
                {
                        _min_pattern_len = std::min (_min_pattern_len, pattern.size ());
                        _max_pattern_len = std::max (_max_pattern_len, pattern.size ());
                        _pattern_lens.push_back (pattern.size ());
                        // TODO: add the pattern to the prefilter here
                        State *prev = _start_state;
                        bool saw_match = false;
                        bool skip_pattern = false;
                        size_t depth = 0;
                        for (const char &c : pattern)
                                {
                                        saw_match = saw_match || prev->is_match ();
                                        if (_match_kind == MatchKind::LEFTMOST_FIRST && saw_match)
                                                {
                                                        // skip to the next pattern
                                                        skip_pattern = true;
                                                        break;
                                                }
                                        _char_set.add_char (c);
                                        State *next = prev->transitions[static_cast<CodePoint>(c)];
                                        if (next == nullptr)
                                                {
                                                        next = add_state (depth);
                                                        prev->transitions[static_cast<CodePoint>(c)] = next;
                                                        if (_ignore_case)
                                                                {
                                                                        prev->transitions[opposite_ascii_case (c)] = next;
                                                                }
                                                }
                                        prev = next;
                                        ++depth;
                                }
                        if (skip_pattern)
                                {
                                        continue;
                                }
                        prev->matches.insert (&pattern);
                }
}

void NFA::add_failure_transitions ()
{
        bool is_leftmost = _match_kind == MatchKind::LEFTMOST_FIRST;
        std::queue<State *> queue{};
        std::set<State *> visited{nullptr};
        for (auto &next : _start_state->transitions)
                {
                        if (visited.contains (next))
                                continue;
                        queue.push (next);
                        visited.insert (next);
                        if (is_leftmost && next->is_match ())
                                next->failed = _dead_state;
                }
        while (!queue.empty ())
                {
                        auto *state = queue.front ();
                        queue.pop ();
                        for (int c = 0; c < 128; ++c)
                                {
                                        State *next = state->transitions[c];
                                        if (visited.contains (next))
                                                continue;
                                        queue.push (next);
                                        visited.insert (next);
                                        if (is_leftmost && next->is_match ())
                                                {
                                                        next->failed = _dead_state;
                                                        continue;
                                                }
                                        State *fail = state->failed;
                                        while (fail->next_state (c) == nullptr)
                                                {
                                                        fail = fail->failed;
                                                }
                                        fail = fail->next_state (c);
                                        next->failed = fail;
                                        copy_matches (fail, next);
                                }
                        if (!is_leftmost)
                                {
                                        copy_matches (_start_state, state);
                                }
                }
}

void NFA::copy_matches (State *src, State *dst)
{
        dst->matches.insert (src->matches.begin (), src->matches.end ());
}

State *NFA::add_state (size_t depth)
{
        auto *state = new State {{nullptr}, {}, _start_state, depth};
        _states.push_back (state);
        return state;
}

void NFA::init_start_state ()
{
        _start_state->failed = _start_state;
}

void NFA::add_start_state_loop ()
{
        for (auto &s : _start_state->transitions)
                {
                        if (s == nullptr)
                                {
                                        s = _start_state;
                                }
                }
}
void NFA::close_start_state_loop_for_leftmost ()
{
        if (_match_kind == MatchKind::LEFTMOST_FIRST && _start_state->is_match ())
                {
                        for (uint8_t c = 0; /**/; ++c)
                                {
                                        if (_start_state->next_state (c) == _start_state)
                                                {
                                                        _start_state->transitions[c] = _dead_state;
                                                }
                                        if (c == 255)
                                                {
                                                        break;
                                                }
                                }
                }
}
void NFA::add_dead_state_loop ()
{
        for (auto &s : _dead_state->transitions)
                {
                        s = _dead_state;
                }

}

}  // namespace automaton