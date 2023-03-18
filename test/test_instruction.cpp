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
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "bits.h"
#include "test_instruction.h"

struct MidRegBytes {
  Byte highByte;
  Byte lowNibble;
  Byte regX;
  Byte regY;
};

struct TestRegisterState {
  Byte regX;
  Byte valX;
  Byte regY;
  Byte valY;
  Byte result;
  Byte expectedVal;
};

static auto
BuildMiddleRegInstruction(const MidRegBytes &rdata) -> Instruction {
  // to produce instructions of the form NxyM
  const Byte high = (rdata.highByte | rdata.regX);
  const Byte low = (rdata.regY << (CHAR_BIT / 2)) | rdata.lowNibble;

  return bits8::fuseBytes(high, low);
}

static auto BuildAddressInstruction(const Byte instrNib, Address addr)
    -> Instruction {
  const Byte shift = 12;

  Instruction code = instrNib;
  code <<= shift;
  code |= addr;

  return code;
}

TestInstruction::TestInstruction()
    : eng(rdev()), byteDist(BYTE_MIN, BYTE_MAX),
      validAddrDist(0, Memory8::memSize), memory_(Memory8::loadAddrDefault),
      regSet_() {}

void TestInstruction::runTests() {
  for (const auto &[desc, func] : functionMap_) {
    std::cout << "Running " << desc << "...";
    std::invoke(func, this);
    std::cout << "PASSED\n";
  }
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

  InstructionSet8 iset(regSet_, memory_);
  regSet_.pc = Memory8::loadAddrDefault;

  for (Address addr = 0; addr < Memory8::memSize; addr++) {
    Instruction opcode = BuildAddressInstruction(instrId, addr);
    iset.DecodeExecuteInstruction(opcode);
    assert((regSet_.pc == addr) && "PC address equality 0x1nnn");
  }
}

// CALL addr
void TestInstruction::Test2nnn() {
  const Byte instrId = 0x2;
  const Address incr = 0x111;

  InstructionSet8 iset(regSet_, memory_);
  regSet_.pc = Memory8::loadAddrDefault;
  while (!regSet_.callStack.empty()) {
    regSet_.callStack.pop();
  }

  for (Address addr = 0; addr < Memory8::memSize; addr += incr) {
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

  InstructionSet8 iset(regSet_, memory_);
  regSet_.pc = Memory8::loadAddrDefault;

  // test Vx == kk
  for (Byte regX = 0; regX < RegisterSet8::regCount; regX++) {
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
  for (Byte regX = 0; regX < RegisterSet8::regCount; regX++) {
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

  InstructionSet8 iset(regSet_, memory_);
  regSet_.pc = Memory8::loadAddrDefault;

  // test Vx == kk
  for (Byte regX = 0; regX < RegisterSet8::regCount; regX++) {
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
  for (Byte regX = 0; regX < RegisterSet8::regCount; regX++) {
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

  InstructionSet8 iset(regSet_, memory_);
  regSet_.pc = Memory8::loadAddrDefault;

  // check Vx != Vy
  for (Byte regX = 0; regX < RegisterSet8::regCount; regX++) {
    for (Byte regY = 0; regY < RegisterSet8::regCount; regY++) {
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
  for (Byte regX = 0; regX < RegisterSet8::regCount; regX++) {
    for (Byte regY = 0; regY < RegisterSet8::regCount; regY++) {
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

  InstructionSet8 iset(regSet_, memory_);
  regSet_.pc = Memory8::loadAddrDefault;

  for (Byte reg = 0; reg < RegisterSet8::regCount; reg++) {
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

  InstructionSet8 iset(regSet_, memory_);
  regSet_.pc = Memory8::loadAddrDefault;

  for (Byte reg = 0; reg < RegisterSet8::regCount; reg++) {
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

void TestInstruction::TestBlock8() {
  Test8xy0();
  Test8xy1();
  Test8xy2();
  Test8xy3();
  Test8xy4();
  Test8xy5();
  Test8xy6();
  Test8xy7();
  Test8xyE();
}

// LD Vx, Vy
void TestInstruction::Test8xy0() {
  const Byte typeCode = 0x0;
  MidRegBytes rdata{TestInstruction::arithmeticCode, typeCode, 0x0, 0x0};

  InstructionSet8 iset(regSet_, memory_);
  regSet_.pc = Memory8::loadAddrDefault;

  for (Byte regX = 0; regX < RegisterSet8::regCount; regX++) {
    for (Byte regY = 0; regY < RegisterSet8::regCount; regY++) {
      for (int val = 0; val <= BYTE_MAX; val++) {
        regSet_.registers.at(regY) = static_cast<Byte>(val);
        rdata.regX = regX;
        rdata.regY = regY;

        Instruction opcode = BuildMiddleRegInstruction(rdata);
        iset.DecodeExecuteInstruction(opcode);
        assert((regSet_.registers.at(regX) == regSet_.registers.at(regY)) &&
               "Register equality 0x8xy0");
      }
    }
  }
}

static auto GenerateErrorText(const std::string &problem,
                              const TestRegisterState &rstate) -> std::string {
  std::stringstream err;
  err << problem << " ";

  err << "Register 0x" << std::hex << static_cast<int>(rstate.regX) << " = 0x"
      << std::hex << static_cast<int>(rstate.valX) << ", ";

  err << "Register 0x" << std::hex << static_cast<int>(rstate.regY) << " = 0x"
      << std::hex << static_cast<int>(rstate.valY) << ", ";

  err << "Output = 0x" << std::hex << static_cast<int>(rstate.result) << ", ";

  err << "Expected = 0x" << std::hex << static_cast<int>(rstate.expectedVal);

  return err.str();
}

// NOLINTBEGIN(readability-function-cognitive-complexity)
void TestInstruction::RunArithmeticTests(const Byte typeCode,
                                         const BinaryOp &binOp,
                                         const BinaryOp &flagOp) {
  MidRegBytes rdata{TestInstruction::arithmeticCode, typeCode, 0x0, 0x0};

  InstructionSet8 iset(regSet_, memory_);
  regSet_.pc = Memory8::loadAddrDefault;

  // test distinct registers and special values
  for (Byte regX = 0; regX < RegisterSet8::flagReg; regX++) {
    for (Byte regY = 0; regY < RegisterSet8::flagReg; regY++) {
      if (regX == regY) {
        continue;
      }
      rdata.regX = regX;
      rdata.regY = regY;

      for (const auto &valX : boundaryBytes) {
        for (const auto &valY : boundaryBytes) {
          regSet_.registers.at(regX) = valX;
          regSet_.registers.at(regY) = valY;
          Instruction opcode = BuildMiddleRegInstruction(rdata);
          iset.DecodeExecuteInstruction(opcode);

          if (regSet_.registers.at(regX) != binOp(valX, valY)) {
            const TestRegisterState currState{regX,
                                              valX,
                                              regY,
                                              valY,
                                              regSet_.registers.at(regX),
                                              binOp(valX, valY)};
            std::stringstream err;
            err << "Operation " << std::hex << opcode << " failed.";
            auto desc = GenerateErrorText(err.str(), currState);
            throw std::runtime_error(desc);
          }

          const auto fval = regSet_.registers.at(RegisterSet8::flagReg);
          if (flagOp && (fval != flagOp(valX, valY))) {
            const TestRegisterState currState{regX, valX, regY,
                                              valY, fval, flagOp(valX, valY)};
            std::stringstream err;
            err << "Operation " << std::hex << opcode
                << " failed with incorrect flag value.";
            auto desc = GenerateErrorText(err.str(), currState);
            throw std::runtime_error(desc);
          }
        }
      }
    }
  }

  const int trials = 1000;

  // test distinct registers and random values
  for (Byte regX = 0; regX < RegisterSet8::flagReg; regX++) {
    for (Byte regY = 0; regY < RegisterSet8::flagReg; regY++) {
      if (regX == regY) {
        continue;
      }
      rdata.regX = regX;
      rdata.regY = regY;
      for (int iter = 0; iter < trials; iter++) {
        const auto valX = byteDist(eng);
        const auto valY = byteDist(eng);

        regSet_.registers.at(regX) = valX;
        regSet_.registers.at(regY) = valY;
        Instruction opcode = BuildMiddleRegInstruction(rdata);
        iset.DecodeExecuteInstruction(opcode);

        if (regSet_.registers.at(regX) != binOp(valX, valY)) {
          const TestRegisterState currState{regX,
                                            valX,
                                            regY,
                                            valY,
                                            regSet_.registers.at(regX),
                                            binOp(valX, valY)};
          std::stringstream err;
          err << "Operation " << std::hex << opcode << " failed.";
          auto desc = GenerateErrorText(err.str(), currState);
          throw std::runtime_error(desc);
        }

        const auto fval = regSet_.registers.at(RegisterSet8::flagReg);
        if (flagOp && (fval != flagOp(valX, valY))) {
          const TestRegisterState currState{regX, valX, regY,
                                            valY, fval, flagOp(valX, valY)};
          std::stringstream err;
          err << "Operation " << std::hex << opcode
              << " failed with incorrect flag value.";
          auto desc = GenerateErrorText(err.str(), currState);
          throw std::runtime_error(desc);
        }
      }
    }
  }

  // test identical registers and all Byte values
  for (Byte reg = 0; reg < RegisterSet8::flagReg; reg++) {
    rdata.regX = reg;
    rdata.regY = reg;
    for (int ival = 0; ival <= BYTE_MAX; ival++) {
      const Byte bval = static_cast<Byte>(ival);
      regSet_.registers.at(reg) = bval;
      Instruction opcode = BuildMiddleRegInstruction(rdata);
      iset.DecodeExecuteInstruction(opcode);

      if (regSet_.registers.at(reg) != binOp(bval, bval)) {
        const TestRegisterState currState{
            reg, bval, reg, bval, regSet_.registers.at(reg), binOp(bval, bval)};
        std::stringstream err;
        err << "Operation " << std::hex << opcode << " failed.";
        auto desc = GenerateErrorText(err.str(), currState);
        throw std::runtime_error(desc);
      }

      const auto fval = regSet_.registers.at(RegisterSet8::flagReg);
      if (flagOp && (fval != flagOp(bval, bval))) {
        const TestRegisterState currState{reg,  bval, reg,
                                          bval, fval, flagOp(bval, bval)};
        std::stringstream err;
        err << "Operation " << std::hex << opcode
            << " failed with incorrect flag value.";
        auto desc = GenerateErrorText(err.str(), currState);
        throw std::runtime_error(desc);
      }
    }
  }
}
// NOLINTEND(readability-function-cognitive-complexity)

// OR Vx, Vy
void TestInstruction::Test8xy1() {
  const Byte typeCode = 0x1;
  auto binOp = [](Byte val1, Byte val2) {
    return static_cast<Byte>(val1 | val2);
  };
  RunArithmeticTests(typeCode, binOp, {});
}

// AND Vx, Vy
void TestInstruction::Test8xy2() {
  const Byte typeCode = 0x2;
  auto binOp = [](Byte val1, Byte val2) {
    return static_cast<Byte>(val1 & val2);
  };
  RunArithmeticTests(typeCode, binOp, {});
}

// XOR Vx, Vy
void TestInstruction::Test8xy3() {
  const Byte typeCode = 0x3;
  auto binOp = [](Byte val1, Byte val2) {
    return static_cast<Byte>(val1 ^ val2);
  };
  RunArithmeticTests(typeCode, binOp, {});
}

// ADD Vx, Vy
void TestInstruction::Test8xy4() {
  const Byte typeCode = 0x4;
  auto binOp = [](Byte val1, Byte val2) {
    return static_cast<Byte>(val1 + val2);
  };
  auto flagOp = [](Byte val1, Byte val2) {
    return ((val1 + val2) > BYTE_MAX) ? 1 : 0;
  };
  RunArithmeticTests(typeCode, binOp, flagOp);
}

// SUB Vx, Vy
void TestInstruction::Test8xy5() {
  const Byte typeCode = 0x5;
  auto binOp = [](Byte val1, Byte val2) {
    return static_cast<Byte>(val1 - val2);
  };
  auto flagOp = [](Byte val1, Byte val2) { return (val1 > val2) ? 1 : 0; };
  RunArithmeticTests(typeCode, binOp, flagOp);
}

// SHR Vx {, Vy}
void TestInstruction::Test8xy6() {
  const Byte typeCode = 0x6;
  auto binOp = [](Byte val1, Byte val2) {
    std::ignore = val2;
    return static_cast<Byte>(val1 >> 1);
  };
  auto flagOp = [](Byte val1, Byte val2) {
    std::ignore = val2;
    return bits8::getLsb(val1);
  };
  RunArithmeticTests(typeCode, binOp, flagOp);
}

// SUBN Vx, Vy
void TestInstruction::Test8xy7() {
  const Byte typeCode = 0x7;
  auto binOp = [](Byte val1, Byte val2) {
    return static_cast<Byte>(val2 - val1);
  };
  auto flagOp = [](Byte val1, Byte val2) { return (val2 > val1) ? 1 : 0; };
  RunArithmeticTests(typeCode, binOp, flagOp);
}

// SHL Vx {, Vy}
void TestInstruction::Test8xyE() {
  const Byte typeCode = 0xE;
  auto binOp = [](Byte val1, Byte val2) {
    std::ignore = val2;
    return static_cast<Byte>(val1 << 1);
  };
  auto flagOp = [](Byte val1, Byte val2) {
    std::ignore = val2;
    return bits8::getMsb(val1);
  };
  RunArithmeticTests(typeCode, binOp, flagOp);
}

// SNE Vx, Vy
void TestInstruction::Test9xy0() {
  const Byte hiByte = 0x90;

  InstructionSet8 iset(regSet_, memory_);
  regSet_.pc = Memory8::loadAddrDefault;

  // check Vx != Vy
  for (Byte regX = 0; regX < RegisterSet8::regCount; regX++) {
    for (Byte regY = 0; regY < RegisterSet8::regCount; regY++) {
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
  for (Byte regX = 0; regX < RegisterSet8::regCount; regX++) {
    for (Byte regY = 0; regY < RegisterSet8::regCount; regY++) {
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

  InstructionSet8 iset(regSet_, memory_);
  regSet_.pc = Memory8::loadAddrDefault;

  for (Address addr = 0; addr < Memory8::memSize; addr++) {
    Instruction opcode = BuildAddressInstruction(instrId, addr);
    iset.DecodeExecuteInstruction(opcode);
    assert((regSet_.regI == addr) && "Set register I 0xAnnn");
  }
}

// JP V0, addr
void TestInstruction::TestBnnn() {
  const Byte instrId = 0xB;

  InstructionSet8 iset(regSet_, memory_);
  regSet_.pc = Memory8::loadAddrDefault;

  for (Address addr = 0; addr < Memory8::memSize; addr++) {
    for (int val = 0; val <= BYTE_MAX; val++) {
      regSet_.registers.at(0) = static_cast<Byte>(val);
      Instruction opcode = BuildAddressInstruction(instrId, addr);
      iset.DecodeExecuteInstruction(opcode);
      assert((regSet_.pc == (addr + static_cast<Byte>(val))) &&
             "Jump sum 0xBnnn");
    }
  }
}

void TestInstruction::TestBlockF() {
  TestFx07();
  TestFx15();
  TestFx18();
  TestFx1E();
  TestFx29();
  TestFx33();
  TestFx55();
  TestFx65();
}

// LD Vx, DT
void TestInstruction::TestFx07() {
  const Byte hiByte = 0xF0;
  const Byte lowByte = 0x07;

  InstructionSet8 iset(regSet_, memory_);
  regSet_.pc = Memory8::loadAddrDefault;

  for (Byte reg = 0; reg < RegisterSet8::regCount; reg++) {
    for (int val = 0; val <= BYTE_MAX; val++) {
      regSet_.regDT = static_cast<Byte>(val);
      Instruction opcode = bits8::fuseBytes((hiByte | reg), lowByte);
      iset.DecodeExecuteInstruction(opcode);
      assert((regSet_.registers.at(reg) == regSet_.regDT) &&
             "Loading delay timer 0xFx07");
    }
  }
}

// LD DT, Vx
void TestInstruction::TestFx15() {
  const Byte hiByte = 0xF0;
  const Byte lowByte = 0x15;

  InstructionSet8 iset(regSet_, memory_);
  regSet_.pc = Memory8::loadAddrDefault;

  for (Byte reg = 0; reg < RegisterSet8::regCount; reg++) {
    for (int val = 0; val <= BYTE_MAX; val++) {
      regSet_.registers.at(reg) = static_cast<Byte>(val);
      Instruction opcode = bits8::fuseBytes((hiByte | reg), lowByte);
      iset.DecodeExecuteInstruction(opcode);
      assert((regSet_.registers.at(reg) == regSet_.regDT) &&
             "Storing delay timer 0xFx15");
    }
  }
}

// LD ST, Vx
void TestInstruction::TestFx18() {
  const Byte hiByte = 0xF0;
  const Byte lowByte = 0x18;

  InstructionSet8 iset(regSet_, memory_);
  regSet_.pc = Memory8::loadAddrDefault;

  for (Byte reg = 0; reg < RegisterSet8::regCount; reg++) {
    for (int val = 0; val <= BYTE_MAX; val++) {
      regSet_.registers.at(reg) = static_cast<Byte>(val);
      Instruction opcode = bits8::fuseBytes((hiByte | reg), lowByte);
      iset.DecodeExecuteInstruction(opcode);
      assert((regSet_.registers.at(reg) == regSet_.regST) &&
             "Storing sound timer 0xFx18");
    }
  }
}

// ADD I, Vx
void TestInstruction::TestFx1E() {
  const Byte hiByte = 0xF0;
  const Byte lowByte = 0x1E;

  InstructionSet8 iset(regSet_, memory_);
  regSet_.pc = Memory8::loadAddrDefault;

  const std::size_t testIter = 1000;
  for (Byte reg = 0; reg < RegisterSet8::regCount; reg++) {
    for (std::size_t iter = 0; iter < testIter; iter++) {
      auto addr = validAddrDist(eng);
      auto val = byteDist(eng);

      regSet_.regI = addr;
      regSet_.registers.at(reg) = val;
      Instruction opcode = bits8::fuseBytes((hiByte | reg), lowByte);
      iset.DecodeExecuteInstruction(opcode);
      assert((regSet_.regI == (addr + val)) && "Add instruction + reg 0xFx1E");
    }
  }
}

// LD F, Vx
void TestInstruction::TestFx29() {
  const std::map<Byte, Address> manualSpriteMap = {
      {0x0, 0x50}, {0x1, 0x55}, {0x2, 0x5A}, {0x3, 0x5F},
      {0x4, 0x64}, {0x5, 0x69}, {0x6, 0x6E}, {0x7, 0x73},
      {0x8, 0x78}, {0x9, 0x7D}, {0xA, 0x82}, {0xB, 0x87},
      {0xC, 0x8C}, {0xD, 0x91}, {0xE, 0x96}, {0xF, 0x9B}};

  const Byte hiByte = 0xF0;
  const Byte lowByte = 0x29;

  InstructionSet8 iset(regSet_, memory_);
  regSet_.pc = Memory8::loadAddrDefault;

  // test valid sprites
  for (Byte reg = 0; reg < RegisterSet8::regCount; reg++) {
    for (const auto &[digit, addr] : manualSpriteMap) {
      regSet_.registers.at(reg) = digit;
      Instruction opcode = bits8::fuseBytes((hiByte | reg), lowByte);
      iset.DecodeExecuteInstruction(opcode);
      assert((regSet_.regI == addr) && "Valid sprite load 0xFx29");
    }
  }

  // test invalid sprites
  const Byte invalid = 0x10;
  for (Byte reg = 0; reg < RegisterSet8::regCount; reg++) {
    try {
      regSet_.registers.at(reg) = invalid;
      Instruction opcode = bits8::fuseBytes((hiByte | reg), lowByte);
      iset.DecodeExecuteInstruction(opcode);
      assert(false && "Invalid sprite load 0xFx29");
    } catch (const std::out_of_range &err) {
      std::ignore = err;
    }
  }
}

static auto GetBcd(Byte val) -> std::vector<Byte> {
  const int base = 10;

  const Byte ones = static_cast<Byte>(val % base);
  const Byte tens = static_cast<Byte>((val / base) % base);
  const Byte hundreds = static_cast<Byte>((val / (base * base)) % base);

  return {hundreds, tens, ones};
}

// LD B, Vx
void TestInstruction::TestFx33() {
  const Byte hiByte = 0xF0;
  const Byte lowByte = 0x33;
  const Byte places = 3;

  InstructionSet8 iset(regSet_, memory_);
  regSet_.pc = Memory8::loadAddrDefault;
  regSet_.regI = Memory8::loadAddrDefault;

  for (Byte reg = 0; reg < RegisterSet8::regCount; reg++) {
    for (int val = 0; val <= BYTE_MAX; val++) {
      const auto bval = static_cast<Byte>(val);
      regSet_.registers.at(reg) = bval;
      Instruction opcode = bits8::fuseBytes((hiByte | reg), lowByte);
      iset.DecodeExecuteInstruction(opcode);

      std::vector<Byte> result;
      memory_.fetchSequence(regSet_.regI, places, result);

      std::vector<Byte> expectVec = GetBcd(bval);
      assert((result.size() == expectVec.size()) && "BCD vector size 0xFx33");
      assert(std::equal(result.begin(), result.end(), expectVec.begin()) &&
             "BCD vector contents 0xFx33");
    }
  }
}

// LD [I], Vx
void TestInstruction::TestFx55() {
  const Byte hiByte = 0xF0;
  const Byte lowByte = 0x55;

  InstructionSet8 iset(regSet_, memory_);
  regSet_.pc = Memory8::loadAddrDefault;
  regSet_.regI = Memory8::loadAddrDefault;

  for (Byte endReg = 0; endReg < RegisterSet8::regCount; endReg++) {
    std::vector<Byte> bvec;
    for (Byte reg = 0; reg <= endReg; reg++) {
      const auto val = byteDist(eng);
      regSet_.registers.at(reg) = val;
      bvec.push_back(val);
    }

    Instruction opcode = bits8::fuseBytes((hiByte | endReg), lowByte);
    iset.DecodeExecuteInstruction(opcode);

    std::vector<Byte> resultVec;
    memory_.fetchSequence(regSet_.regI, endReg + 1, resultVec);

    assert((bvec.size() == resultVec.size()) && "Memory vector size 0xFx55");
    assert(std::equal(bvec.begin(), bvec.end(), resultVec.begin()) &&
           "Memory vector contents 0xFx55");
  }
}

// LD Vx, [I]
void TestInstruction::TestFx65() {
  const Byte hiByte = 0xF0;
  const Byte lowByte = 0x65;

  InstructionSet8 iset(regSet_, memory_);
  regSet_.pc = Memory8::loadAddrDefault;
  regSet_.regI = Memory8::loadAddrDefault;

  for (Byte endReg = 0; endReg < RegisterSet8::regCount; endReg++) {
    std::vector<Byte> bvec;
    Address addr = regSet_.regI;
    for (Byte reg = 0; reg <= endReg; reg++) {
      const auto val = byteDist(eng);
      memory_.setByte(addr, val);
      bvec.push_back(val);
      addr++;
    }

    Instruction opcode = bits8::fuseBytes((hiByte | endReg), lowByte);
    iset.DecodeExecuteInstruction(opcode);

    std::vector<Byte> resultVec(regSet_.registers.begin(),
                                regSet_.registers.begin() + endReg + 1);

    assert((bvec.size() == resultVec.size()) && "Memory vector size 0xFx65");
    assert(std::equal(bvec.begin(), bvec.end(), resultVec.begin()) &&
           "Memory vector contents 0xFx65");
  }
}
