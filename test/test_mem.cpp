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

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <iterator>

#include "test_mem.h"

TestMemory::TestMemory()
    : eng(rdev()), byteDist(BYTE_MIN, BYTE_MAX), wordDist(WORD_MIN, WORD_MAX) {}

void TestMemory::runTests() {
  for (const auto &[desc, func] : functionMap_) {
    std::cout << "Running " << desc << "...";
    std::invoke(func, this);
    std::cout << "PASSED\n";
  }
}

void TestMemory::fetchByteBoundsTest() {
  Memory8 mem{Memory8::loadAddrDefault};

  // testing bad bounds (should throw)
  for (const auto &addr : badAddrBounds) {
    try {
      std::ignore = mem.fetchByte(addr);
      throw std::runtime_error("Invalid memory access permitted");
    } catch (const std::runtime_error &err) {
      std::ignore = err;
      continue;
    }
  }

  // testing good bounds (shouldn't throw)
  for (const auto &addr : goodAddrBounds) {
    std::ignore = mem.fetchByte(addr);
  }
}

void TestMemory::setByteBoundsTest() {
  Memory8 mem{Memory8::loadAddrDefault};
  const Byte val = 0x1F;

  // testing bad bounds (should throw)
  for (const auto &addr : badAddrBounds) {
    try {
      mem.setByte(addr, val);
      throw std::runtime_error("Invalid memory assignment permitted");
    } catch (const std::runtime_error &err) {
      std::ignore = err;
      continue;
    }
  }

  // testing good bounds (shouldn't throw)
  for (const auto &addr : goodAddrBounds) {
    mem.setByte(addr, val);
  }
}

void TestMemory::fetchInstructionBoundsTest() {
  Memory8 mem{Memory8::loadAddrDefault};

  // testing bad bounds (should throw)
  for (const auto &addr : badAddrBounds) {
    try {
      std::ignore = mem.fetchInstruction(addr);
      throw std::runtime_error("Invalid memory access permitted");
    } catch (const std::runtime_error &err) {
      std::ignore = err;
      continue;
    }
  }

  // testing good bounds (shouldn't throw)
  for (const auto &addr : goodInstructionBounds) {
    std::ignore = mem.fetchInstruction(addr);
  }
}

void TestMemory::fetchSequenceBoundsTest() {
  Memory8 mem{Memory8::loadAddrDefault};

  const Word size = 0x10;
  std::vector<Byte> byteVec{size};

  // testing bad bounds (should throw)
  for (const auto &addr : badAddrBounds) {
    try {
      mem.fetchSequence(addr, size, byteVec);
      throw std::runtime_error("Invalid memory access permitted");
    } catch (const std::runtime_error &err) {
      std::ignore = err;
      continue;
    }
  }

  // good bounds determined by length of sequence
  mem.fetchSequence(Memory8::loadAddrDefault, size, byteVec);
  byteVec.clear();

  mem.fetchSequence(Memory8::memSize - size, size, byteVec);
  byteVec.clear();

  mem.fetchSequence((Memory8::loadAddrDefault + Memory8::memSize) / 2, size,
                    byteVec);
  byteVec.clear();
}

void TestMemory::setSequenceBoundsTest() {
  Memory8 mem{Memory8::loadAddrDefault};

  const Word size = 0x10;
  std::vector<Byte> byteVec{size};

  auto callRand = [this]() { return byteDist(eng); };
  std::generate_n(std::back_inserter(byteVec), size, callRand);

  // testing bad bounds (should throw)
  for (const auto &addr : badAddrBounds) {
    try {
      mem.setSequence(addr, size, byteVec);
      throw std::runtime_error("Invalid memory access permitted");
    } catch (const std::runtime_error &err) {
      std::ignore = err;
      continue;
    }
  }

  // good bounds determined by length of sequence
  mem.setSequence(Memory8::loadAddrDefault, size, byteVec);
  mem.setSequence(Memory8::memSize - size, size, byteVec);
  mem.setSequence((Memory8::loadAddrDefault + Memory8::memSize) / 2, size,
                  byteVec);
}

void TestMemory::inverseGetSetSingleTest() {
  Memory8 mem{Memory8::loadAddrDefault};

  for (std::size_t i = 0; i < randomTestCount_; i++) {
    // make sure we can't go out of bounds by limiting address range
    const Address addr =
        static_cast<Address>(byteDist(eng)) + Memory8::loadAddrDefault;
    Byte val = byteDist(eng);
    mem.setByte(addr, val);
    auto result = mem.fetchByte(addr);

    assert(val == result && "retrieved byte == assigned byte");
  }
}

void TestMemory::inverseGetSetSequenceTest() {
  Memory8 mem{Memory8::loadAddrDefault};
  auto callRand = [this]() { return byteDist(eng); };

  for (std::size_t i = 0; i < randomTestCount_; i++) {
    // make sure we can't go out of bounds by limiting address range
    const Address addr =
        static_cast<Address>(byteDist(eng)) + Memory8::loadAddrDefault;
    Word vsize = static_cast<Word>(byteDist(eng));
    std::vector<Byte> byteVec;
    byteVec.reserve(vsize);
    std::vector<Byte> resultVec;
    resultVec.reserve(vsize);

    std::generate_n(std::back_inserter(byteVec), vsize, callRand);

    mem.setSequence(addr, vsize, byteVec);
    mem.fetchSequence(addr, vsize, resultVec);

    assert((byteVec.size() == resultVec.size()) && "equal sequence lengths");
    assert(std::equal(byteVec.begin(), byteVec.end(), resultVec.begin()) &&
           "equal sequence contents");
  }
}
// test program loading and dumping via string streams

// test load/dump inverse property via arrays/vectors
// NOTE: use std::generate_n from <algorithm> to fill the arrays, will need
// lambda to wrap rng call
