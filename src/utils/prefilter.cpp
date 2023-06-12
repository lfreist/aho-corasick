/**
 * Copyright 2023, Leon Freist (https://github.com/lfreist)
 * Author: Leon Freist <freist.leon@gmail.com>
 * 
 * This file is part of builddir.
 */

#include <cctype>
#include <ac/utils/prefilter.h>

unsigned char opposite_ascii_case(unsigned char c) {
        if (c >= 'A' && c <= 'Z') {
                return std::tolower (c);
        } else if (c >= 'a' && c <= 'z') {
                return std::toupper (c);
        }
        return c;
}