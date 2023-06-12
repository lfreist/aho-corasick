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

#include <ac/search.h>
#include <ac/utils/charset.h>
#include <ac/utils/prefilter.h>

class State;
class NFA;

/**
 * @brief
 */
class State {
  friend class NFA;
 public:
  explicit State (State *failed, State *nfa_fail, size_t depth);

  // size_t memory_usage();
  [[nodiscard]]
  bool is_match () const;
  [[nodiscard]]
  State *next_state (CodePoint code_point) const;

  void add_transition (CodePoint code_point, State *state);
  void add_match (const std::string *match);

  [[nodiscard]]
  const std::set<const std::string *>& get_matches() const;

 private:
  State* _transitions[256] {nullptr};
  std::set<const std::string *> _matches{};
  State *_failed;
  State *NFA_FAIL;
  size_t _depth{0};
};

/**
 * @brief
 */
class NFA {
 public:
  /**
   * @brief Constructing an Aho-Corasick NFA.
   * @param patterns This is a pointer instead of a reference on purpose: We want to add pointers of the patterns to the
   *  States as matches which only works when getting the pointers to the actual strings. By passing a reference of the
   *  vector, the pointers to its internal strings are not valid anymore once the reference gets out of scope.
   * @param match_kind
   * @param ascii_i_case
   */
  NFA (const std::vector<std::string> &patterns, MatchKind match_kind, bool ascii_i_case);
  ~NFA();

 // private:
  void build_trie (const std::vector<std::string> &patterns);
  void add_failure_transitions ();
  void init_start_state();
  void add_start_state_loop();
  void close_start_state_loop_for_leftmost();
  void add_dead_state_loop();

  /**
   * @brief Add a state the the NFA.
   *
   * Make sure, that _fail_state and _start_state are successfully initialized before calling this function.
   * Both are initialized within the constructor and thus calling this function should be safe.
   * @param depth
   * @return
   */
  State *add_state (size_t depth);

  static void copy_matches(State* src, State* dst);

  MatchKind _match_kind;
  /// The charset of the given pattern. It is constructed during NFA compiling
  CharSet _char_set;
  State *_fail_state {new State(nullptr, nullptr, 0)};
  State *_start_state {new State(nullptr, _fail_state, 0)};
  State *_dead_state {new State(nullptr, _fail_state, 0)};
  /// The order of regular states
  std::vector<State*> _states;
  std::vector<size_t> _pattern_lens;
  size_t _min_pattern_len;
  size_t _max_pattern_len;
  bool _ignore_case;
};

#endif //_NFA_H_
