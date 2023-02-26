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

#ifndef EMU8_MEMORY_H
#define EMU8_MEMORY_H

#include <array>
#include <istream>
#include <ostream>
#include <string>
#include <vector>

#include "common.h"

class Memory8 {
public:
  // the Chip-8 only has 4k total memory
  static constexpr std::size_t memSize = 0x1000;
  static constexpr std::size_t loadAddrDefault = 0x200;
  static constexpr Address spriteBegin = 0x50;
  static constexpr Address spriteLen = 5;

  explicit Memory8(std::size_t memBase);

  // all set and fetch methods are bounds-checked, throwing a runtime
  // exception if the starting (or ending) address is illegal

  // retrieve a single byte from memory at address addr
  [[nodiscard]] auto fetchByte(Address addr) const -> Byte;

  // retrieve two sequential bytes from memory, combining them into a big-endian
  // word to be interpreted as a Chip-8 instruction
  [[nodiscard]] auto fetchInstruction(Address addr) const -> Instruction;

  // retrieve a sequence of bytes of length size from memory, starting at addr
  void fetchSequence(Address addr, Word size, std::vector<Byte> &buf) const;

  // set the value of the byte in memory at address addr to val
  void setByte(Address addr, Byte val);

  // set the value of a sequence of bytes in memory to the values specified in
  // buf, starting at address addr and continuing for size bytes
  void setSequence(Address addr, Word size, const std::vector<Byte> &buf);

  // load a program image into memory from input stream, starting at memLow_
  void loadProgram(std::istream &progStream);

  // dump full memory image to specified output stream for debugging
  void dumpCore(std::ostream &coreStream) const;

private:
  const std::size_t memLow_;
  std::array<Byte, memSize> memory_;

  void fillTextSprites();
};

#endif /* EMU8_MEMORY_H */
