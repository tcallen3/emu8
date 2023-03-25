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

#include <functional>
#include <map>
#include <set>

#include "instruction_set.h"
#include "interface.h"
#include "memory.h"
#include "register_set.h"
#include "test.h"

class TestInstruction;
using InstructionMemFn = void (TestInstruction::*)();
using BinaryOp = std::function<Byte(Byte, Byte)>;

class TestInstruction : public Test {
public:
  TestInstruction();
  void runTests() override;

private:
  void RunArithmeticTests(Byte typeCode, const BinaryOp &binOp,
                          const BinaryOp &flagOp);

  void Test00EE();
  void Test1nnn();
  void Test2nnn();
  void Test3xkk();
  void Test4xkk();
  void Test5xy0();
  void Test6xkk();
  void Test7xkk();

  void TestBlock8();
  void Test8xy0();
  void Test8xy1();
  void Test8xy2();
  void Test8xy3();
  void Test8xy4();
  void Test8xy5();
  void Test8xy6();
  void Test8xy7();
  void Test8xyE();

  void Test9xy0();
  void TestAnnn();
  void TestBnnn();

  void TestBlockF();
  void TestFx07();
  void TestFx15();
  void TestFx18();
  void TestFx1E();
  void TestFx29();
  void TestFx33();
  void TestFx55();
  void TestFx65();

  static constexpr Byte arithmeticCode = 0x80;
  const std::set<Byte> boundaryBytes = {0x0, 0x1, 0x8F, 0xFE, 0xFF};

  std::random_device rdev = {};
  std::default_random_engine eng;
  std::uniform_int_distribution<Byte> byteDist;
  std::uniform_int_distribution<Address> validAddrDist;

  Memory8 memory_;
  RegisterSet8 regSet_;
  Interface8 interface_;

  const std::map<std::string, InstructionMemFn> functionMap_ = {
      {"Instruction 00EE", &TestInstruction::Test00EE},
      {"Instruction 1nnn", &TestInstruction::Test1nnn},
      {"Instruction 2nnn", &TestInstruction::Test2nnn},
      {"Instruction 3xkk", &TestInstruction::Test3xkk},
      {"Instruction 4xkk", &TestInstruction::Test4xkk},
      {"Instruction 5xy0", &TestInstruction::Test5xy0},
      {"Instruction 6xkk", &TestInstruction::Test6xkk},
      {"Instruction 7xkk", &TestInstruction::Test7xkk},
      {"Instruction Block 8000", &TestInstruction::TestBlock8},
      {"Instruction 9xy0", &TestInstruction::Test9xy0},
      {"Instruction Annn", &TestInstruction::TestAnnn},
      {"Instruction Bnnn", &TestInstruction::TestBnnn},
      {"Instruction Block F000", &TestInstruction::TestBlockF}};
};

#endif /* TEST_INSTRUCTION_H */
