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

#include <iostream>
#include <vector>

#include "test.h"
#include "test_bits.h"
#include "test_mem.h"

auto main() -> int {

  std::vector<Test *> testPtrs;
  TestBits tbits;
  TestMemory tmem;

  testPtrs.push_back(&tbits);
  testPtrs.push_back(&tmem);

  for (const auto &ptr : testPtrs) {
    ptr->runTests();
  }

  std::cout << "Finished testing\n";
  return 0;
}
