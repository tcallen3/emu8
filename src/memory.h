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
#include <string>
#include <vector>

#include "common.h"

class Memory8 {
public:
  static constexpr std::size_t loadAddrDefault = 0x200;

  explicit Memory8(std::size_t memBase);

  // all fetch methods are bounds-checked, throwing a runtime
  // exception if the starting (or ending) address is illegal
  [[nodiscard]] auto FetchWord(Address addr) const -> Word;
  [[nodiscard]] auto FetchByte(Address addr) const -> Byte;
  void FetchSequence(Address addr, Word size, std::vector<Byte> &buf) const;

  // all set methods are bounds-checked, throwing a runtime
  // exception if the starting (or ending) address is illegal
  void SetWord(Address addr, Word val);
  void SetByte(Address addr, Byte val);
  void SetSequence(Address addr, Word size, const std::vector<Byte> &buf);

  // dump full memory image to specified file for debugging
  void CoreDump(const std::string &coreFile) const;

private:
  // the Chip-8 only has 4k total memory
  static constexpr std::size_t memSize_ = 0x1000;

  const std::size_t memLow_;
  std::array<Byte, memSize_> memory_;
};

#endif /* EMU8_MEMORY_H */
