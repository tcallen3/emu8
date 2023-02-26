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

#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <string>
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

static auto GetSingleRegNibble(Instruction opcode) -> Byte {
  // for use with instructions of the form nxnn where x is the target register
  auto [high, low] = bits8::splitWord(opcode);
  std::ignore = low;
  return bits8::lowNibble(high);
}

InstructionSet8::InstructionSet8(RegisterSet8 &reg, Memory8 &mem)
    : eng(rdev()), byteDist(BYTE_MIN, BYTE_MAX), regSet_{reg}, memory_{mem} {}

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
  const auto addr = bits8::maskAddress(opcode_);
  regSet_.pc = addr;
}

void InstructionSet8::Execute2nnn() {
  // CALL addr - call subroutine at nnn
  if (regSet_.callStack.size() >= RegisterSet8::stackSize) {
    throw std::overflow_error("stack overflow");
  }

  const auto addr = bits8::maskAddress(opcode_);
  regSet_.callStack.push(addr);
}

void InstructionSet8::Execute3xkk() {
  // SE Vx, byte - skip next instruction if Vx == kk
  auto [high, bytekk] = bits8::splitWord(opcode_);
  const auto nibX = bits8::lowNibble(high);

  if (regSet_.registers[nibX] == bytekk) {
    regSet_.pc += 2;
  }
}

void InstructionSet8::Execute4xkk() {
  // SNE Vx, byte - skip next instruction if Vx != kk
  auto [high, bytekk] = bits8::splitWord(opcode_);
  const auto nibX = bits8::lowNibble(high);

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
  const auto nibX = bits8::lowNibble(high);

  regSet_.registers[nibX] = bytekk;
}

void InstructionSet8::Execute7xkk() {
  // ADD Vx, byte - set Vx = Vx + kk
  auto [high, bytekk] = bits8::splitWord(opcode_);
  const auto nibX = bits8::lowNibble(high);

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

void InstructionSet8::Execute8xy5() {
  // SUB Vx, Vy - set Vx = Vx - Vy, set VF = NOT borrow
  auto [nibX, nibY] = GetMiddleNibbles(opcode_);

  const auto valX = regSet_.registers[nibX];
  const auto valY = regSet_.registers[nibY];
  regSet_.registers.back() = (valX > valY) ? 1 : 0;

  regSet_.registers[nibX] = valX - valY;
}

void InstructionSet8::Execute8xy6() {
  // SHR Vx {, Vy} - Set Vx = Vx SHR 1
  auto [nibX, nibY] = GetMiddleNibbles(opcode_);

  std::ignore = nibY;
  const auto valX = regSet_.registers[nibX];

  regSet_.registers.back() = bits8::getLsb(valX);
  regSet_.registers[nibX] = (valX >> 1);
}

void InstructionSet8::Execute8xy7() {
  // SUBN Vx, Vy - set Vx = Vy - Vx, set VF = NOT borrow
  auto [nibX, nibY] = GetMiddleNibbles(opcode_);

  const auto valX = regSet_.registers[nibX];
  const auto valY = regSet_.registers[nibY];
  regSet_.registers.back() = (valY > valX) ? 1 : 0;

  regSet_.registers[nibX] = valY - valX;
}

void InstructionSet8::Execute8xyE() {
  // SHL Vx {, Vy} - Set Vx = Vx SHR 1
  auto [nibX, nibY] = GetMiddleNibbles(opcode_);

  std::ignore = nibY;
  const auto valX = regSet_.registers[nibX];

  regSet_.registers.back() = bits8::getMsb(valX);
  regSet_.registers[nibX] = (valX << 1);
}

void InstructionSet8::Execute9xy0() {
  // SNE Vx, Vy - skip next instruction if Vx != Vy
  auto [nibX, nibY] = GetMiddleNibbles(opcode_);

  const auto valX = regSet_.registers[nibX];
  const auto valY = regSet_.registers[nibY];

  if (valX != valY) {
    regSet_.pc += 2;
  }
}

void InstructionSet8::ExecuteAnnn() {
  // LD I, addr - set I = nnn
  const auto addr = bits8::maskAddress(opcode_);

  regSet_.regI = addr;
}

void InstructionSet8::ExecuteBnnn() {
  // JP V0, addr -  jump to location nnn + V0
  const auto addr = bits8::maskAddress(opcode_);
  const auto val0 = regSet_.registers[0];

  regSet_.pc = addr + val0;
}

void InstructionSet8::ExecuteCxkk() {
  // RND Vx, byte - set Vx = random byte AND kk
  auto [high, bytekk] = bits8::splitWord(opcode_);
  const auto regX = bits8::lowNibble(high);

  regSet_.registers[regX] = byteDist(eng) & bytekk;
}

void InstructionSet8::ExecuteDxyn() {
  // DRW Vx, Vy, nibble - display n-byte sprite starting at memory location I at
  // (Vx, Vy) on screen, set VF = collision

  // FIXME: implement
}

void InstructionSet8::ExecuteEx9E() {
  // SKP Vx - skip next instruction if key with the value of Vx is pressed

  // FIXME: implement
}

void InstructionSet8::ExecuteExA1() {
  // SKNP Vx - skip next instruction if key with the value of Vx is not pressed

  // FIXME: implement
}

void InstructionSet8::ExecuteFx07() {
  // LD Vx, DT - set Vx = delay timer value
  const auto regX = GetSingleRegNibble(opcode_);

  regSet_.registers[regX] = regSet_.regDT;
}

void InstructionSet8::ExecuteFx0A() {
  // LD Vx, K - wait for a key press and store the value of the key in Vx

  // FIXME: implment
}

void InstructionSet8::ExecuteFx15() {
  // LD DT, Vx - set delay timer = Vx
  // FIXME: implment delay countdown
  const auto regX = GetSingleRegNibble(opcode_);
  regSet_.regDT = regSet_.registers[regX];
}

void InstructionSet8::ExecuteFx18() {
  // LD ST, Vx - set sound timer = Vx
  // FIXME: implment sound countdown
  const auto regX = GetSingleRegNibble(opcode_);
  regSet_.regST = regSet_.registers[regX];
}

void InstructionSet8::ExecuteFx1E() {
  // ADD I, Vx - set I = I + Vx
  const auto regX = GetSingleRegNibble(opcode_);
  regSet_.regI += regSet_.registers[regX];
}

void InstructionSet8::ExecuteFx29() {
  // LD F, Vx - set I = location of sprite for digit Vx
  constexpr auto maxNibble = 0xF;
  const auto regX = GetSingleRegNibble(opcode_);
  const auto valX = regSet_.registers[regX];

  if (valX > maxNibble) {
    throw std::out_of_range("Invalid sprite address request for value = " +
                            std::to_string(valX) + " (limit 0xF -> 15)");
  }

  regSet_.regI =
      static_cast<Address>(Memory8::spriteBegin + (Memory8::spriteLen * valX));
}

void InstructionSet8::ExecuteFx33() {
  // LD B, Vx - store binary coded decimal representation of Vx in memory
  // locations I, I+1, and I+2
  const auto regX = GetSingleRegNibble(opcode_);
  const auto valX = regSet_.registers[regX];

  const Word base = 10;
  const Word bound = 1000;
  Word divisor = 1;

  Address addr = regSet_.regI + 2;
  while (divisor < bound) {
    Byte digit = static_cast<Byte>((valX / divisor) % base);
    memory_.setByte(addr, digit);

    divisor *= base;
    addr--;
  }
}

void InstructionSet8::ExecuteFx55() {
  // LD [I], Vx - store registers V0 through Vx in memory starting at location I
  const auto regX = GetSingleRegNibble(opcode_);

  // add one to register value since transfer is inclusive, [0,X] vs. [0,X)
  std::vector<Byte> regVals(regSet_.registers.begin(),
                            regSet_.registers.begin() + regX + 1);

  memory_.setSequence(regSet_.regI, static_cast<Word>(regVals.size()), regVals);
}

void InstructionSet8::ExecuteFx65() {
  // LD Vx, [I] - read registers V0 through Vx from memory starting at I
  const auto regX = GetSingleRegNibble(opcode_);
  std::vector<Byte> regVals;

  // add one to register value since transfer is inclusive, [0,X] vs. [0,X)
  memory_.fetchSequence(regSet_.regI, regX + 1, regVals);
  std::copy(regVals.begin(), regVals.end(), regSet_.registers.begin());
}
