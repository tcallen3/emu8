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
#include "test_instruction.h"

TestInstruction::TestInstruction()
    : eng(rdev()), byteDist(BYTE_MIN, BYTE_MAX),
      memory_(Memory8::loadAddrDefault), regSet_() {}

void TestInstruction::runTests() {
  for (const auto &[desc, func] : functionMap_) {
    std::cout << "Running " << desc << "...";
    std::invoke(func, this);
    std::cout << "PASSED\n";
  }
}

static auto BuildAddressInstruction(const Byte instrNib, Address addr)
    -> Instruction {
  const Byte shift = 12;

  Instruction code = instrNib;
  code <<= shift;
  code |= addr;

  return code;
}

// RET
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
  regSet_.pc = Memory8::loadAddrDefault;
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

// JP addr
void TestInstruction::Test1nnn() {
  const Byte instrId = 0x1;
  const Address limit = 0x1000;

  InstructionSet8 iset(regSet_, memory_);
  regSet_.pc = Memory8::loadAddrDefault;

  for (Address addr = 0; addr < limit; addr++) {
    Instruction opcode = BuildAddressInstruction(instrId, addr);
    iset.DecodeExecuteInstruction(opcode);
    assert((regSet_.pc == addr) && "PC address equality 0x1nnn");
  }
}

// CALL addr
void TestInstruction::Test2nnn() {
  const Byte instrId = 0x2;
  const Address limit = 0x1000;
  const Address incr = 0x111;

  InstructionSet8 iset(regSet_, memory_);
  regSet_.pc = Memory8::loadAddrDefault;
  while (!regSet_.callStack.empty()) {
    regSet_.callStack.pop();
  }

  for (Address addr = 0; addr < limit; addr += incr) {
    const auto prevSize = regSet_.callStack.size();
    const auto prevPc = regSet_.pc;
    Instruction opcode = BuildAddressInstruction(instrId, addr);
    iset.DecodeExecuteInstruction(opcode);
    assert((regSet_.pc == addr) && "PC set to address 0x2nnn");
    assert((regSet_.callStack.top() == prevPc) &&
           "Old PC saved to stack 0x2nnn");
    assert((regSet_.callStack.size() > prevSize) && "Stack incremented 0x2nnn");
  }

  // test stack overflow
  try {
    const Instruction opcode = 0x2123;
    iset.DecodeExecuteInstruction(opcode);
    assert(false && "Stack overflow 0x2nnn");
  } catch (const std::overflow_error &err) {
    std::ignore = err;
  }
}

// SE Vx, byte
void TestInstruction::Test3xkk() {
  const Byte hiByte = 0x30;
  const Byte rcount = static_cast<Byte>(RegisterSet8::regCount);

  InstructionSet8 iset(regSet_, memory_);
  regSet_.pc = Memory8::loadAddrDefault;

  // test Vx == kk
  for (Byte regX = 0; regX < rcount; regX++) {
    for (int kk = 0; kk <= BYTE_MAX; kk++) {
      Byte val = static_cast<Byte>(kk);
      regSet_.registers.at(regX) = val;
      const auto oldPc = regSet_.pc;
      Instruction opcode = bits8::fuseBytes((hiByte | regX), val);
      iset.DecodeExecuteInstruction(opcode);
      assert((regSet_.pc == (oldPc + 2)) && "Equal register 0x3xkk");
    }
  }

  // test Vx != kk
  for (Byte regX = 0; regX < rcount; regX++) {
    for (int kk = 0; kk <= BYTE_MAX; kk++) {
      Byte val = static_cast<Byte>(kk);
      regSet_.registers.at(regX) = (val == 0) ? 1 : val - 1;
      const auto oldPc = regSet_.pc;
      Instruction opcode = bits8::fuseBytes((hiByte | regX), val);
      iset.DecodeExecuteInstruction(opcode);
      assert((regSet_.pc == oldPc) && "Unequal register 0x3xkk");
    }
  }
}

// SNE Vx, byte
void TestInstruction::Test4xkk() {
  const Byte hiByte = 0x40;
  const Byte rcount = static_cast<Byte>(RegisterSet8::regCount);

  InstructionSet8 iset(regSet_, memory_);
  regSet_.pc = Memory8::loadAddrDefault;

  // test Vx == kk
  for (Byte regX = 0; regX < rcount; regX++) {
    for (int kk = 0; kk <= BYTE_MAX; kk++) {
      Byte val = static_cast<Byte>(kk);
      regSet_.registers.at(regX) = val;
      const auto oldPc = regSet_.pc;
      Instruction opcode = bits8::fuseBytes((hiByte | regX), val);
      iset.DecodeExecuteInstruction(opcode);
      assert((regSet_.pc == oldPc) && "Equal register 0x4xkk");
    }
  }

  // test Vx != kk
  for (Byte regX = 0; regX < rcount; regX++) {
    for (int kk = 0; kk <= BYTE_MAX; kk++) {
      Byte val = static_cast<Byte>(kk);
      regSet_.registers.at(regX) = (val == 0) ? 1 : val - 1;
      const auto oldPc = regSet_.pc;
      Instruction opcode = bits8::fuseBytes((hiByte | regX), val);
      iset.DecodeExecuteInstruction(opcode);
      assert((regSet_.pc == (oldPc + 2)) && "Unequal register 0x4xkk");
    }
  }
}

// SE Vx, Vy
void TestInstruction::Test5xy0() {
  const Byte hiByte = 0x50;
  const Byte rcount = static_cast<Byte>(RegisterSet8::regCount);

  InstructionSet8 iset(regSet_, memory_);
  regSet_.pc = Memory8::loadAddrDefault;

  // check Vx != Vy
  for (Byte regX = 0; regX < rcount; regX++) {
    for (Byte regY = 0; regY < rcount; regY++) {
      if (regX == regY) {
        continue;
      }
      const auto xval = byteDist(eng);
      auto yval = byteDist(eng);
      while (yval == xval) {
        yval = byteDist(eng);
      }

      regSet_.registers.at(regX) = xval;
      regSet_.registers.at(regY) = yval;
      // internal assert for safety
      assert(regSet_.registers.at(regX) != regSet_.registers.at(regY));

      const auto oldPc = regSet_.pc;
      Instruction opcode =
          bits8::fuseBytes((hiByte | regX), regY << (CHAR_BIT / 2));
      iset.DecodeExecuteInstruction(opcode);
      assert((oldPc == regSet_.pc) && "Unequal registers increment 0x5xy0");
    }
  }

  // check Vx == Vy
  for (Byte regX = 0; regX < rcount; regX++) {
    for (Byte regY = 0; regY < rcount; regY++) {
      const auto val = byteDist(eng);
      regSet_.registers.at(regX) = val;
      regSet_.registers.at(regY) = val;

      const auto oldPc = regSet_.pc;
      Instruction opcode =
          bits8::fuseBytes((hiByte | regX), regY << (CHAR_BIT / 2));
      iset.DecodeExecuteInstruction(opcode);
      assert((regSet_.pc == (oldPc + 2)) && "Equal registers increment 0x5xy0");
    }
  }
}

// LD Vx, byte
void TestInstruction::Test6xkk() {
  const Byte hiByte = 0x60;
  const Byte rcount = static_cast<Byte>(RegisterSet8::regCount);

  InstructionSet8 iset(regSet_, memory_);
  regSet_.pc = Memory8::loadAddrDefault;

  for (Byte reg = 0; reg < rcount; reg++) {
    for (int val = BYTE_MIN; val <= BYTE_MAX; val++) {
      Instruction opcode =
          bits8::fuseBytes((hiByte | reg), static_cast<Byte>(val));
      iset.DecodeExecuteInstruction(opcode);
      assert((regSet_.registers.at(reg) == val) &&
             "Register assignment 0x6xkk");
    }
  }
}

// ADD Vx, byte
void TestInstruction::Test7xkk() {
  const Byte hiByte = 0x70;
  const Byte rcount = static_cast<Byte>(RegisterSet8::regCount);

  InstructionSet8 iset(regSet_, memory_);
  regSet_.pc = Memory8::loadAddrDefault;

  for (Byte reg = 0; reg < rcount; reg++) {
    for (int rval = 0; rval <= BYTE_MAX; rval++) {
      Byte rvb = static_cast<Byte>(rval);
      for (int ival = 0; ival <= BYTE_MAX; ival++) {
        Byte ivb = static_cast<Byte>(ival);
        regSet_.registers.at(reg) = rvb;
        Instruction opcode = bits8::fuseBytes((hiByte | reg), ivb);
        iset.DecodeExecuteInstruction(opcode);
        Byte sum = static_cast<Byte>(rvb + ivb);
        assert((regSet_.registers.at(reg) == sum) &&
               "Register + immediate sum 0x7xkk");
      }
    }
  }
}

// SNE Vx, Vy
void TestInstruction::Test9xy0() {
  const Byte hiByte = 0x90;
  const Byte rcount = static_cast<Byte>(RegisterSet8::regCount);

  InstructionSet8 iset(regSet_, memory_);
  regSet_.pc = Memory8::loadAddrDefault;

  // check Vx != Vy
  for (Byte regX = 0; regX < rcount; regX++) {
    for (Byte regY = 0; regY < rcount; regY++) {
      if (regX == regY) {
        continue;
      }
      const auto xval = byteDist(eng);
      auto yval = byteDist(eng);
      while (yval == xval) {
        yval = byteDist(eng);
      }

      regSet_.registers.at(regX) = xval;
      regSet_.registers.at(regY) = yval;
      // internal assert for safety
      assert(regSet_.registers.at(regX) != regSet_.registers.at(regY));

      const auto oldPc = regSet_.pc;
      Instruction opcode =
          bits8::fuseBytes((hiByte | regX), regY << (CHAR_BIT / 2));
      iset.DecodeExecuteInstruction(opcode);
      assert((regSet_.pc == (oldPc + 2)) &&
             "Unequal registers increment 0x9xy0");
    }
  }

  // check Vx == Vy
  for (Byte regX = 0; regX < rcount; regX++) {
    for (Byte regY = 0; regY < rcount; regY++) {
      const auto val = byteDist(eng);
      regSet_.registers.at(regX) = val;
      regSet_.registers.at(regY) = val;

      const auto oldPc = regSet_.pc;
      Instruction opcode =
          bits8::fuseBytes((hiByte | regX), regY << (CHAR_BIT / 2));
      iset.DecodeExecuteInstruction(opcode);
      assert((regSet_.pc == oldPc) && "Equal registers increment 0x9xy0");
    }
  }
}

// LD I, addr
void TestInstruction::TestAnnn() {
  const Byte instrId = 0xA;
  const Address limit = 0x1000;

  InstructionSet8 iset(regSet_, memory_);
  regSet_.pc = Memory8::loadAddrDefault;

  for (Address addr = 0; addr < limit; addr++) {
    Instruction opcode = BuildAddressInstruction(instrId, addr);
    iset.DecodeExecuteInstruction(opcode);
    assert((regSet_.regI == addr) && "Set register I 0xAnnn");
  }
}

// JP V0, addr
void TestInstruction::TestBnnn() {
  const Byte instrId = 0xB;
  const Address limit = 0x1000;

  InstructionSet8 iset(regSet_, memory_);
  regSet_.pc = Memory8::loadAddrDefault;

  for (Address addr = 0; addr < limit; addr++) {
    for (int val = 0; val <= BYTE_MAX; val++) {
      regSet_.registers.at(0) = static_cast<Byte>(val);
      Instruction opcode = BuildAddressInstruction(instrId, addr);
      iset.DecodeExecuteInstruction(opcode);
      assert((regSet_.pc == (addr + static_cast<Byte>(val))) &&
             "Jump sum 0xBnnn");
    }
  }
}
