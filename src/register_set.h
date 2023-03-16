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

#ifndef EMU8_REGISTER_SET_H
#define EMU8_REGISTER_SET_H

#include <array>
#include <stack>

#include "common.h"

struct RegisterSet8 {
  static constexpr std::size_t stackSize = 16;
  static constexpr Byte regCount = 16;
  static constexpr Byte flagReg = 0xF;
  Byte regST = {0x0};
  Byte regDT = {0x0};
  Address regI = {0x0};
  Address pc = {0x0};
  std::array<Byte, regCount> registers = {};
  std::stack<Address> callStack = {};
};

#endif /* EMU8_REGISTER_SET_H */
