/**
 * Copyright 2023, Leon Freist (https://github.com/lfreist)
 * Author: Leon Freist <freist.leon@gmail.com>
 * 
 * This file is part of builddir.
 */

#include <ac/utils/charset.h>
#include <ac/utils/prefilter.h>
#include <cctype>

CharSet::CharSet (bool ignore_case) : _ignore_case (ignore_case)
{}

uint8_t CharSet::size () const
{
        return _size;
}

void CharSet::add_char (unsigned char c)
{
        if (_ignore_case)
                {
                        _mapping[_size] = std::tolower (c);
                        _reverse_mapping[opposite_ascii_case (c)] = _size;
                }
        else
                {
                        _mapping[_size] = c;
                }
        _reverse_mapping[c] = _size;
        _size++;
}

unsigned char CharSet::get_char (CodePoint code_point) const
{
        return _mapping[code_point];
}

CodePoint CharSet::get_code_point (unsigned char c) const
{
        if (_ignore_case)
                {
                        return _reverse_mapping[std::tolower (c)];
                }
        return _reverse_mapping[c];
}