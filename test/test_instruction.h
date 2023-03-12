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

#ifndef TEST_INSTRUCTION_H
#define TEST_INSTRUCTION_H

#include <map>

#include "instruction_set.h"
#include "memory.h"
#include "register_set.h"
#include "test.h"

class TestInstruction;
using InstructionMemFn = void (TestInstruction::*)();

class TestInstruction : public Test {
public:
  TestInstruction();
  void runTests() override;

private:
  void Test00EE();
  void Test1nnn();
  void Test2nnn();
  void Test3xkk();
  void Test4xkk();
  void Test5xy0();
  void Test6xkk();
  void Test7xkk();

  // FIXME add tests for 0x8XXX

  void Test9xy0();
  void TestAnnn();
  void TestBnnn();

  // FIXME add tests for 0xFXXX

  std::random_device rdev = {};
  std::default_random_engine eng;
  std::uniform_int_distribution<Byte> byteDist;

  Memory8 memory_;
  RegisterSet8 regSet_;

  const std::map<std::string, InstructionMemFn> functionMap_ = {
      {"Instruction 00EE", &TestInstruction::Test00EE},
      {"Instruction 1nnn", &TestInstruction::Test1nnn},
      {"Instruction 2nnn", &TestInstruction::Test2nnn},
      {"Instruction 3xkk", &TestInstruction::Test3xkk},
      {"Instruction 4xkk", &TestInstruction::Test4xkk},
      {"Instruction 5xy0", &TestInstruction::Test5xy0},
      {"Instruction 6xkk", &TestInstruction::Test6xkk},
      {"Instruction 7xkk", &TestInstruction::Test7xkk},
      {"Instruction 9xy0", &TestInstruction::Test9xy0},
      {"Instruction Annn", &TestInstruction::TestAnnn},
      {"Instruction Bnnn", &TestInstruction::TestBnnn}};
};

#endif /* TEST_INSTRUCTION_H */
