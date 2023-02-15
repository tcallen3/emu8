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

#include "bits.h"

#include "test_bits.h"

TestBits::TestBits()
    : eng(rdev()), byteDist(BYTE_MIN, BYTE_MAX), wordDist(WORD_MIN, WORD_MAX) {}

void TestBits::runTests() {
  for (const auto &[desc, func] : functionMap_) {
    std::cout << "Running " << desc << "...";
    std::invoke(func, this);
    std::cout << "PASSED\n";
  }
}

void TestBits::inverseFuseSplitTest() {

  for (std::size_t i = 0; i < randomTestCount_; i++) {
    Byte msb = byteDist(eng);
    Byte lsb = byteDist(eng);

    auto result = bits8::splitWord(bits8::fuseBytes(msb, lsb));

    assert((msb == result.first) && "High-byte equality for split(fuse())");
    assert((lsb == result.second) && "Low-byte equality for split(fuse())");
  }
}

void TestBits::fuseLowByteTest() {
  const Byte mask = 0xFF;
  for (std::size_t i = 0; i < randomTestCount_; i++) {
    Byte msb = byteDist(eng);
    Byte lsb = byteDist(eng);

    auto fused = bits8::fuseBytes(msb, lsb);

    assert((((fused & mask) ^ lsb) == 0) &&
           "Low-byte equality for fuseBytes()");
  }
}

void TestBits::fuseHighByteTest() {
  for (std::size_t i = 0; i < randomTestCount_; i++) {
    Byte msb = byteDist(eng);
    Byte lsb = byteDist(eng);

    auto fused = bits8::fuseBytes(msb, lsb);

    assert((((fused >> CHAR_BIT) ^ msb) == 0) &&
           "High-byte equality for fuseBytes()");
  }
}

void TestBits::splitBytesTest() {
  const Byte mask = 0xFF;
  for (std::size_t i = 0; i < randomTestCount_; i++) {
    Word test = wordDist(eng);
    auto [msb, lsb] = bits8::splitWord(test);

    assert((((test & mask) ^ lsb) == 0) && "Low-byte equality for splitWord()");
    assert((((test >> CHAR_BIT) ^ msb) == 0) &&
           "High-byte equality for splitWord()");
  }
}
