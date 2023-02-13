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
#include <iostream>
#include <random>

#include "bits.h"
#include "common.h"

#include "test_bits.h"

static constexpr std::size_t RANDOM_TEST_COUNT = 1000;

// FIXME: this doesn't work because of endianness...
// We'll need to test fuse and split separately and dumbly
void inverse_fuse_split_test() {
  std::cout << "Testing inverse property of fuseBytes(splitWord())...";
  std::random_device r;
  std::default_random_engine eng(r());
  std::uniform_int_distribution<Word> dist(WORD_MIN, WORD_MAX);

  for (std::size_t i = 0; i < RANDOM_TEST_COUNT; i++) {
    Word val = dist(eng);
    auto bpair = bits8::splitWord(val);

    Byte msb = bpair.first;
    Byte lsb = bpair.second;

    Word result = bits8::fuseBytes(msb, lsb);

    assert(val == result);
  }
}
