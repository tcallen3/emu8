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

#include "config_parser.h"

#include "test.h"
#include "test_bits.h"
#include "test_instruction.h"
#include "test_mem.h"

auto main() -> int {

  std::vector<Test *> testPtrs;
  TestBits tbits;
  TestMemory tmem;
  TestInstruction tinstr;

  testPtrs.push_back(&tbits);
  testPtrs.push_back(&tmem);
  testPtrs.push_back(&tinstr);

  for (const auto &ptr : testPtrs) {
    ptr->runTests();
  }

  ConfigParser config;
  const auto iniMap = config.ParseFile("config.ini");
  for (const auto &[key, val] : iniMap) {
    std::cout << static_cast<int>(key) << " -> " << static_cast<int>(val)
              << std::endl;
  }

  std::cout << "Finished testing\n";
  return 0;
}
