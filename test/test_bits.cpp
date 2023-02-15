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

void inverse_fuse_split_test() {
  std::cout << "Testing inverse property of splitWord(fuseBytes())...";
  std::random_device rdev;
  std::default_random_engine eng(rdev());
  std::uniform_int_distribution<Byte> dist(BYTE_MIN, BYTE_MAX);

  for (std::size_t i = 0; i < RANDOM_TEST_COUNT; i++) {
    Byte msb = dist(eng);
    Byte lsb = dist(eng);

    auto result = bits8::splitWord(bits8::fuseBytes(msb, lsb));

    assert(msb == result.first);
    assert(lsb == result.second);
  }
}
