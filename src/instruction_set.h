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

#ifndef EMU8_INSTRUCTION_SET_H
#define EMU8_INSTRUCTION_SET_H

#include <map>
#include <random>
#include <set>

#include "memory.h"
#include "register_set.h"

class InstructionSet8;
using InstructionFn = void (InstructionSet8::*)();
using CodeMap = std::map<Word, InstructionFn>;

class InstructionSet8 {
public:
  InstructionSet8(RegisterSet8 &reg, Memory8 &mem);
  void DecodeExecuteInstruction(Instruction opcode);

private:
  std::random_device rdev = {};
  std::default_random_engine eng;
  std::uniform_int_distribution<Byte> byteDist;

  Instruction opcode_ = {};
  RegisterSet8 &regSet_;
  Memory8 &memory_;

  // most significant nibbles for opcodes completely determined by this value
  const std::set<Byte> msnSet = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6,
                                 0x7, 0x9, 0xA, 0xB, 0xC, 0xD};

  // most significant nibbles for opcodes determined by this value and lowest
  // byte (remaining opcodes are 0x8nnn and depend on only most and least
  // significant nibble)
  const std::set<Byte> lowByteSet = {0x0, 0xE, 0xF};

  const CodeMap codeMapping = {
      {0x1, &InstructionSet8::Execute1nnn},
      {0x2, &InstructionSet8::Execute2nnn},
      {0x3, &InstructionSet8::Execute3xkk},
      {0x4, &InstructionSet8::Execute4xkk},
      {0x5, &InstructionSet8::Execute5xy0},
      {0x6, &InstructionSet8::Execute6xkk},
      {0x7, &InstructionSet8::Execute7xkk},
      {0x9, &InstructionSet8::Execute9xy0},
      {0xA, &InstructionSet8::ExecuteAnnn},
      {0xB, &InstructionSet8::ExecuteBnnn},
      {0xC, &InstructionSet8::ExecuteCxkk},
      {0xD, &InstructionSet8::ExecuteDxyn},

      {0x00E0, &InstructionSet8::Execute00E0},
      {0x00EE, &InstructionSet8::Execute00EE},

      {0x8000, &InstructionSet8::Execute8xy0},
      {0x8001, &InstructionSet8::Execute8xy1},
      {0x8002, &InstructionSet8::Execute8xy2},
      {0x8003, &InstructionSet8::Execute8xy3},
      {0x8004, &InstructionSet8::Execute8xy4},
      {0x8005, &InstructionSet8::Execute8xy5},
      {0x8006, &InstructionSet8::Execute8xy6},
      {0x8007, &InstructionSet8::Execute8xy7},
      {0x800E, &InstructionSet8::Execute8xyE},

      {0xE09E, &InstructionSet8::ExecuteEx9E},
      {0xE0A1, &InstructionSet8::ExecuteExA1},

      {0xF007, &InstructionSet8::ExecuteFx07},
      {0xF00A, &InstructionSet8::ExecuteFx0A},
      {0xF015, &InstructionSet8::ExecuteFx15},
      {0xF018, &InstructionSet8::ExecuteFx18},
      {0xF01E, &InstructionSet8::ExecuteFx1E},
      {0xF029, &InstructionSet8::ExecuteFx29},
      {0xF033, &InstructionSet8::ExecuteFx33},
      {0xF055, &InstructionSet8::ExecuteFx55},
      {0xF065, &InstructionSet8::ExecuteFx65},
  };

  void Execute00E0();
  void Execute00EE();

  void Execute1nnn();
  void Execute2nnn();
  void Execute3xkk();
  void Execute4xkk();
  void Execute5xy0();
  void Execute6xkk();
  void Execute7xkk();

  void Execute8xy0();
  void Execute8xy1();
  void Execute8xy2();
  void Execute8xy3();
  void Execute8xy4();
  void Execute8xy5();
  void Execute8xy6();
  void Execute8xy7();
  void Execute8xyE();

  void Execute9xy0();
  void ExecuteAnnn();
  void ExecuteBnnn();
  void ExecuteCxkk();
  void ExecuteDxyn();

  void ExecuteEx9E();
  void ExecuteExA1();

  void ExecuteFx07();
  void ExecuteFx0A();
  void ExecuteFx15();
  void ExecuteFx18();
  void ExecuteFx1E();
  void ExecuteFx29();
  void ExecuteFx33();
  void ExecuteFx55();
  void ExecuteFx65();
};

#endif /* EMU8_INSTRUCTION_SET_H */
