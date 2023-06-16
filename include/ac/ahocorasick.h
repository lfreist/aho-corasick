/**
 * Copyright 2023, Leon Freist (https://github.com/lfreist)
 * Author: Leon Freist <freist.leon@gmail.com>
 * 
 * This file is part of x-search.
 */

#ifndef _AHOCORASICK_H_
#define _AHOCORASICK_H_

#include <ac/search.h>
#include <ac/nfa/nfa.h>

#include <vector>
#include <string>

enum AutomatonType {
  DFA,
  NFA
};

struct Result {
  const std::string match;
  size_t start;
  size_t end;
};

std::ostream &operator<<(std::ostream &os, Result const &result);

template <typename automaton_type>
class AhoCorasick {
 public:
  AhoCorasick(std::vector<std::string> patterns, MatchKind match_kind);

  std::vector<Result> find_all(std::string input);

 private:
  std::vector<std::string> _patterns;
  MatchKind _match_kind;
  automaton_type _automaton;
};

#endif //_AHOCORASICK_H_
