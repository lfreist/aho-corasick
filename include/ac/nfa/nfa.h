/**
 * Copyright 2023, Leon Freist (https://github.com/lfreist)
 * Author: Leon Freist <freist.leon@gmail.com>
 * 
 * This file is part of lfreist/aho-cohasic.
 */

#ifndef _NFA_H_
#define _NFA_H_

#include <vector>
#include <utility>
#include <cstdint>
#include <string>
#include <set>
#include <unordered_map>
#include <limits>

#include <ac/search.h>
#include <ac/utils/charset.h>
#include <ac/utils/prefilter.h>

namespace automaton
{

class NFA;

/**
 * @brief
 */
struct State {
  State *transitions[128]{nullptr};
  std::set<const std::string *> matches{};
  State *failed{nullptr};
  size_t depth{0};

  [[nodiscard]]
  bool is_match () const;

  [[nodiscard]]
  State *next_state (CodePoint code_point) const;
};

/**
 * @brief
 */
class NFA {
 public:
  /**
   * @brief Constructing an Aho-Corasick NFA.
   * @param patterns
   * @param match_kind
   * @param ascii_i_case
   */
  NFA (const std::vector<std::string> &patterns, MatchKind match_kind, bool ascii_i_case);
  ~NFA ();

  // private:
  void build_trie (const std::vector<std::string> &patterns);
  void add_failure_transitions ();
  void init_start_state ();
  void add_start_state_loop ();
  void close_start_state_loop_for_leftmost ();
  void add_dead_state_loop ();

  /**
   * @brief Add a state the the NFA.
   *
   * Make sure, that and _start_state are successfully initialized before calling this function.
   * Both are initialized within the constructor and thus calling this function should be safe.
   * @param depth
   * @return
   */
  State *add_state (size_t depth);

  static void copy_matches (State *src, State *dst);

  MatchKind _match_kind;
  /// The charset of the given pattern. It is constructed during NFA compiling
  CharSet _char_set;
  State *_start_state{new State};
  State *_dead_state{new State};
  /// The order of regular states
  std::vector<State *> _states{};
  std::vector<size_t> _pattern_lens{};
  size_t _min_pattern_len{std::numeric_limits<size_t>::max ()};
  size_t _max_pattern_len{0};
  bool _ignore_case{false};
};

}  // namespace automaton

#endif //_NFA_H_
