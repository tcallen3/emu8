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

#ifndef TEST_BITS_H
#define TEST_BITS_H

#include <map>
#include <random>

#include "common.h"
#include "test.h"

class TestBits;
using BitsMemFn = void (TestBits::*)();

class TestBits : public Test {
public:

  TestBits();
  void runTests() override;

private:
  void inverseFuseSplitTest();
  void fuseLowByteTest();
  void fuseHighByteTest();
  void splitBytesTest();

  std::random_device rdev = {};
  std::default_random_engine eng;
  std::uniform_int_distribution<Byte> byteDist;
  std::uniform_int_distribution<Word> wordDist;

  static constexpr std::size_t randomTestCount_ = 1000;
  const std::map<std::string, BitsMemFn> functionMap_ = {
      {"Inverse Fuse-Split", &TestBits::inverseFuseSplitTest},
      {"Fuse Low Byte", &TestBits::fuseLowByteTest},
      {"Fuse High Byte", &TestBits::fuseHighByteTest},
      {"Split Bytes", &TestBits::splitBytesTest}};
};
#endif /* TEST_BITS_H */
