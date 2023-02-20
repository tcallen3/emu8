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

#include <stdexcept>
#include <tuple>

#include "bits.h"
#include "instruction_set.h"

static auto GetMiddleNibbles(Instruction opcode) -> BytePair {
  // for word nxyn, return x, y
  auto [high, low] = bits8::splitWord(opcode);
  auto nibX = bits8::lowNibble(high);
  auto nibY = bits8::highNibble(low);

  return {nibX, nibY};
}

InstructionSet8::InstructionSet8(RegisterSet8 &reg, Memory8 &mem)
    : regSet_{reg}, memory_{mem} {}

void InstructionSet8::Execute00E0() {
  // CLS - clear the display
  // FIXME: implement
}

void InstructionSet8::Execute00EE() {
  // RET - return from subroutine
  if (regSet_.callStack.empty()) {
    throw std::underflow_error("stack underflow");
  }

  regSet_.pc = regSet_.callStack.top();
  regSet_.callStack.pop();
}

void InstructionSet8::Execute1nnn() {
  // JP addr - jump to location nnn
  auto addr = bits8::maskAddress(opcode_);
  regSet_.pc = addr;
}

void InstructionSet8::Execute2nnn() {
  // CALL addr - call subroutine at nnn
  if (regSet_.callStack.size() >= RegisterSet8::stackSize) {
    throw std::overflow_error("stack overflow");
  }

  auto addr = bits8::maskAddress(opcode_);
  regSet_.callStack.push(addr);
}

void InstructionSet8::Execute3xkk() {
  // SE Vx, byte - skip next instruction if Vx == kk
  auto [high, bytekk] = bits8::splitWord(opcode_);
  auto nibX = bits8::lowNibble(high);

  if (regSet_.registers[nibX] == bytekk) {
    regSet_.pc += 2;
  }
}

void InstructionSet8::Execute4xkk() {
  // SNE Vx, byte - skip next instruction if Vx != kk
  auto [high, bytekk] = bits8::splitWord(opcode_);
  auto nibX = bits8::lowNibble(high);

  if (regSet_.registers[nibX] != bytekk) {
    regSet_.pc += 2;
  }
}

void InstructionSet8::Execute5xy0() {
  // SE Vx, Vy - skip next instruction if Vx == Vy
  auto [nibX, nibY] = GetMiddleNibbles(opcode_);

  if (regSet_.registers[nibX] == regSet_.registers[nibY]) {
    regSet_.pc += 2;
  }
}

void InstructionSet8::Execute6xkk() {
  // LD Vx, byte - set Vx = kk
  auto [high, bytekk] = bits8::splitWord(opcode_);
  auto nibX = bits8::lowNibble(high);

  regSet_.registers[nibX] = bytekk;
}

void InstructionSet8::Execute7xkk() {
  // ADD Vx, byte - set Vx = Vx + kk
  auto [high, bytekk] = bits8::splitWord(opcode_);
  auto nibX = bits8::lowNibble(high);

  // no overflow tracking
  regSet_.registers[nibX] = regSet_.registers[nibX] + bytekk;
}

void InstructionSet8::Execute8xy0() {
  // LD Vx, Vy - set Vx = Vy
  auto [nibX, nibY] = GetMiddleNibbles(opcode_);

  regSet_.registers[nibX] = regSet_.registers[nibY];
}

void InstructionSet8::Execute8xy1() {
  // OR Vx, Vy - set Vx = Vx OR Vy
  auto [nibX, nibY] = GetMiddleNibbles(opcode_);

  regSet_.registers[nibX] |= regSet_.registers[nibY];
}

void InstructionSet8::Execute8xy2() {
  // AND Vx, Vy - set Vx = Vx AND Vy
  auto [nibX, nibY] = GetMiddleNibbles(opcode_);

  regSet_.registers[nibX] &= regSet_.registers[nibY];
}

void InstructionSet8::Execute8xy3() {
  // XOR Vx, Vy - set Vx = Vx XOR Vy
  auto [nibX, nibY] = GetMiddleNibbles(opcode_);

  regSet_.registers[nibX] ^= regSet_.registers[nibY];
}

void InstructionSet8::Execute8xy4() {
  // ADD Vx, Vy - set Vx = Vx + Vy, set VF = carry
  const Byte maxByte = 0xFF;
  auto [nibX, nibY] = GetMiddleNibbles(opcode_);

  Word sum = static_cast<Word>(regSet_.registers[nibX]) +
             static_cast<Word>(regSet_.registers[nibY]);

  regSet_.registers.back() = (sum > maxByte) ? 1 : 0;

  auto [high, low] = bits8::splitWord(sum);
  std::ignore = high;
  regSet_.registers[nibX] = low;
}

void InstructionSet8::Execute8xy5() {}

void InstructionSet8::Execute8xy6() {}

void InstructionSet8::Execute8xy7() {}

void InstructionSet8::Execute8xyE() {}

void InstructionSet8::Execute9xy0() {}

void InstructionSet8::ExecuteAnnn() {}

void InstructionSet8::ExecuteBnnn() {}

void InstructionSet8::ExecuteCxkk() {}

void InstructionSet8::ExecuteDxyn() {}

void InstructionSet8::ExecuteEx9E() {}

void InstructionSet8::ExecuteExA1() {}

void InstructionSet8::ExecuteFx07() {}

void InstructionSet8::ExecuteFx0A() {}

void InstructionSet8::ExecuteFx15() {}

void InstructionSet8::ExecuteFx18() {}

void InstructionSet8::ExecuteFx1E() {}

void InstructionSet8::ExecuteFx29() {}

void InstructionSet8::ExecuteFx33() {}

void InstructionSet8::ExecuteFx55() {}

void InstructionSet8::ExecuteFx65() {}
