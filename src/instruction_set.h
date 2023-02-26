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

#ifndef EMU8_INSTRUCTION_SET
#define EMU8_INSTRUCTION_SET

#include <random>

#include "memory.h"
#include "register_set.h"

class InstructionSet8;
using InstructionFn = void (InstructionSet8::*)();

class InstructionSet8 {
public:
  InstructionSet8(RegisterSet8 &reg, Memory8 &mem);

  void SetOpcode(Instruction opcode) { opcode_ = opcode; } // needed??
  // InstructionFn DecodeExecuteInstruction(Instruction opcode);

  // NOTE: make these private?
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

private:
  std::random_device rdev = {};
  std::default_random_engine eng;
  std::uniform_int_distribution<Byte> byteDist;

  Instruction opcode_ = {};
  RegisterSet8 &regSet_;
  Memory8 &memory_;
};

#endif /* EMU8_INSTRUCTION_SET */
