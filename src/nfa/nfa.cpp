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
#include <numeric>

#include <iostream>

State::State (State *failed, State *nfa_fail, size_t depth) : _failed (failed), NFA_FAIL (nfa_fail), _depth (depth)
{}

void State::add_match (const std::string *match)
{
        _matches.insert (match);
}

bool State::is_match () const
{
        return !_matches.empty ();
}

void State::add_transition (CodePoint code_point, State *state)
{
        _transitions[code_point] = state;
}

State *State::next_state (CodePoint code_point) const
{
        return _transitions[code_point] == nullptr ? NFA_FAIL : _transitions[code_point];
}

const std::set<const std::string *> &State::get_matches () const
{
        return _matches;
}

// ===== NFA ===========================================================================================================

NFA::NFA (const std::vector<std::string> &patterns, MatchKind match_kind, bool ascii_i_case)
        : _match_kind (match_kind), _ignore_case (ascii_i_case)
{
        // setup FAIL
        _fail_state->NFA_FAIL = _fail_state;
        _fail_state->_failed = _start_state;
        // setup START
        _start_state->_failed = _start_state;
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
        delete _fail_state;
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
                                        State *next = prev->next_state (c);
                                        if (next == _fail_state)
                                                {
                                                        next = add_state (depth);
                                                        prev->add_transition (c, next);
                                                        if (_ignore_case)
                                                                {
                                                                        prev->add_transition (opposite_ascii_case (c), next);
                                                                }
                                                }
                                        prev = next;
                                        ++depth;
                                }
                        if (skip_pattern)
                                {
                                        continue;
                                }
                        prev->add_match (&pattern);
                }
}

void NFA::add_failure_transitions ()
{
        bool is_leftmost = _match_kind == MatchKind::LEFTMOST_FIRST;
        std::queue<State *> queue;
        std::set<State *> visited;
        for (int c = 0; c < 256; ++c)
                {
                        State *next = _start_state->next_state (c);
                        // TODO: c++20 should have visited.contains(), right?
                        if (next == _start_state || visited.find (next) != visited.end ())
                                {
                                        continue;
                                }
                        queue.push (next);
                        visited.insert (next);
                        if (is_leftmost && next->is_match ())
                                {
                                        next->_failed = _dead_state;
                                }
                }
        while (!queue.empty ())
                {
                        auto state = queue.front ();
                        queue.pop ();
                        for (int c = 0; c < 256; ++c)
                                {
                                        State *next = state->next_state (c);
                                        if (visited.find (next) != visited.end ())
                                                {
                                                        continue;
                                                }
                                        queue.push (next);
                                        visited.insert (next);
                                        if (is_leftmost && next->is_match ())
                                                {
                                                        state->_failed = _dead_state;
                                                        continue;
                                                }
                                        auto *fail = state->_failed;
                                        while (fail->next_state (c) == _fail_state)
                                                {
                                                        fail = fail->_failed;
                                                }
                                        fail = fail->next_state (c);
                                        next->_failed = fail;
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
        dst->_matches.insert (src->_matches.begin (), src->_matches.end ());
}

State *NFA::add_state (size_t depth)
{
        State* state = new State(_start_state, _fail_state, depth);
        _states.push_back (state);
        return state;
}

void NFA::init_start_state ()
{
        _start_state->_failed = _start_state;
        for (uint8_t c = 0; /**/ ; ++c)
                {
                        _start_state->add_transition (c, _fail_state);
                        if (c == 255)
                                {
                                        break;
                                }
                }
}

void NFA::add_start_state_loop ()
{
        for (uint8_t c = 0; /**/; ++c)
                {
                        if (_start_state->next_state (c) == _fail_state)
                                {
                                        _start_state->add_transition (c, _start_state);
                                }
                        if (c == 255)
                                {
                                        break;
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
                                                        _start_state->add_transition (c, _dead_state);
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
        for (uint8_t c = 0; /**/; ++c)
                {
                        _dead_state->add_transition (c, _dead_state);
                        if (c == 255)
                                {
                                        break;
                                }
                }
}
