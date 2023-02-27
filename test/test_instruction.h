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

class TestInstruction;
using InstructionMemFn = void (TestInstruction::*)();

class TestInstruction : public Test {
public:
  TestInstruction();
  void runTests() override;

private:
  Memory8 memory_;
  RegisterSet8 regSet_;

  const std::map<std::string, InstructionMemFn> functionMap_ = {};
};

#endif /* TEST_INSTRUCTION_H */
