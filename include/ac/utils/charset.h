/**
 * Copyright 2023, Leon Freist (https://github.com/lfreist)
 * Author: Leon Freist <freist.leon@gmail.com>
 * 
 * This file is part of builddir.
 */

#ifndef _CHARSET_H_
#define _CHARSET_H_

#include <cstdint>

using CodePoint = uint8_t;

/**
 * @brief A charset is a set of all characters contained by patterns
 */
class CharSet {
 public:
  CharSet (bool ignore_case = false);

  /**
   * @brief Get the internal code point of a char c using the _mapping data.
   *
   * @param c
   * @return
   */
  [[nodiscard]]
  CodePoint get_code_point (unsigned char c) const;

  /**
   * @brief Get the char of an internal code point using _mapping data.
   * @param c
   * @return
   */
  [[nodiscard]]
  unsigned char get_char (CodePoint code_point) const;

  /**
   * @brief Get the size of the charset.
   * @return
   */
  [[nodiscard]]
  uint8_t size () const;

  /**
   * @brief Add a char to the charset
   * @param c
   */
  void add_char (unsigned char c);

 private:
  /// size of the charset
  uint8_t _size {1};
  /// used for mapping a char to a code point of the charset
  CodePoint _mapping[256] {0};
  /// used for mapping a code point of the charset to a char
  unsigned char _reverse_mapping[256] {0};

  bool _ignore_case {false};
};

#endif //_CHARSET_H_
