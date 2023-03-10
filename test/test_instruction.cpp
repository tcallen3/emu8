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

#include "test_instruction.h"

TestInstruction::TestInstruction()
    : memory_(Memory8::loadAddrDefault), regSet_() {}

void TestInstruction::runTests() {
  for (const auto &[desc, func] : functionMap_) {
    std::cout << "Running " << desc << "...";
    std::invoke(func, this);
    std::cout << "PASSED\n";
  }
}

static Instruction BuildAddressInstruction(Byte instrNib, Address addr) {
  const Byte shift = 12;

  Instruction code = instrNib;
  code <<= shift;
  code |= addr;

  return code;
}

void TestInstruction::Test00EE() {
  // load up stack pointer
  const std::size_t count = 16;
  const Address incr = 0x111;
  Address addr = 0x0;

  while (!regSet_.callStack.empty()) {
    regSet_.callStack.pop();
  }
  regSet_.pc = 0x0;

  for (std::size_t i = 0; i < count; i++) {
    regSet_.callStack.push(addr);
    addr += incr;
  }

  InstructionSet8 iset(regSet_, memory_);
  const Instruction opcode = 0x00EE;

  // test normal flow
  for (std::size_t i = 0; i < count; i++) {
    const auto memVal = regSet_.callStack.top();
    const auto prevSize = regSet_.callStack.size();

    iset.DecodeExecuteInstruction(opcode);

    assert((regSet_.pc == memVal) && "PC address equality 0x00EE");
    assert((regSet_.callStack.size() < prevSize) && "Stack pop result 0x00EE");
  }

  // test underflow
  try {
    iset.DecodeExecuteInstruction(opcode);
    assert(false && "Stack underflow 0x00EE");
  } catch (const std::underflow_error &err) {
    std::ignore = err;
  }
}

void TestInstruction::Test1nnn() {
  const Byte instrId = 0x1;
  const Address limit = 0x1000;

  InstructionSet8 iset(regSet_, memory_);

  for (Address addr = 0; addr < limit; addr++) {
    Instruction opcode = BuildAddressInstruction(instrId, addr);
    iset.DecodeExecuteInstruction(opcode);
    assert((regSet_.pc == addr) && "PC address equality 1nnn");
  }
}

void TestInstruction::Test2nnn() {}

void TestInstruction::Test3xkk() {}

void TestInstruction::Test4xkk() {}

void TestInstruction::Test5xy0() {}

void TestInstruction::Test6xkk() {}

void TestInstruction::Test7xkk() {}

void TestInstruction::Test9xy0() {}

void TestInstruction::TestAnnn() {}

void TestInstruction::TestBnnn() {}

void TestInstruction::TestCxkk() {}
