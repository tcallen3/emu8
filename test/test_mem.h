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

#ifndef TEST_MEM_H
#define TEST_MEM_H

#include <map>
#include <random>
#include <string>

#include "common.h"
#include "memory.h"
#include "test.h"

class TestMemory;
using MemoryMemFn = void (TestMemory::*)();

class TestMemory : public Test {
public:
  TestMemory();
  void runTests() override;

private:
  void fetchByteBoundsTest();
  void setByteBoundsTest();
  void fetchInstructionBoundsTest();
  void fetchSequenceBoundsTest();
  void setSequenceBoundsTest();

  std::random_device rdev = {};
  std::default_random_engine eng;
  std::uniform_int_distribution<Byte> byteDist;
  std::uniform_int_distribution<Word> wordDist;

  static constexpr std::size_t randomTestCount_ = 1000;
  std::vector<Address> badAddrBounds = {0x0, Memory8::loadAddrDefault,
                                        Memory8::memSize, Memory8::memSize + 1};
  std::vector<Address> goodAddrBounds = {
      Memory8::loadAddrDefault, Memory8::loadAddrDefault + 1,
      Memory8::memSize - 1, Memory8::memSize - 2,
      (Memory8::loadAddrDefault + Memory8::memSize) / 2};
  std::vector<Address> goodInstructionBounds = {
      Memory8::loadAddrDefault, Memory8::loadAddrDefault + 1,
      Memory8::memSize - 2, (Memory8::loadAddrDefault + Memory8::memSize) / 2};

  const std::map<std::string, MemoryMemFn> functionMap_ = {
      {"Byte fetch bounds", &TestMemory::fetchByteBoundsTest},
      {"Byte set bounds", &TestMemory::setByteBoundsTest},
      {"Instruction fetch bounds", &TestMemory::fetchInstructionBoundsTest},
      {"Sequence fetch bounds", &TestMemory::fetchSequenceBoundsTest},
      {"Sequence set bounds", &TestMemory::setSequenceBoundsTest}};
};

#endif /* TEST_MEM_H */
