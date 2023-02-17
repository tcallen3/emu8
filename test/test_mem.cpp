/*
 * emu8 - a C++ Chip-8 emulation program
 * Copyright (C) 2023 Thomas Allen
 *
 * Contact: allen.thomas.c@gmail.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include <cassert>
#include <functional>
#include <iostream>

#include "memory.h"

#include "test_memory.h"

TestMemory::TestMemory()
    : eng(rdev()), byteDist(BYTE_MIN, BYTE_MAX), wordDist(WORD_MIN, WORD_MAX) {}

void TestBits::runTests() {
  for (const auto &[desc, func] : functionMap_) {
    std::cout << "Running " << desc << "...";
    std::invoke(func, this);
    std::cout << "PASSED\n";
  }
}

// access and single set functions: test < memlow, test @ memlow, test > 4k,
// test memsize - 1, test valid interval

// same range tests for sequence access/set functions (accounting for sequence
// length)

// test inverse property for get/set single functions

// test inverse property for sequence get/set functions

// test program loading and dumping via string streams

// test load/dump inverse property via string streams
// NOTE: use std::generate_n from <algorithm> to fill the streams, will need
// lambda to wrap rng call
