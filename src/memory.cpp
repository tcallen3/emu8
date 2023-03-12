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
#include <filesystem>
#include <iterator>
#include <sstream>
#include <stdexcept>

#include "bits.h"
#include "memory.h"

static void reportInvalidAccess(Address addr) {
  std::stringstream errStream;
  errStream << "Invalid memory access: ";
  errStream << std::hex << addr;
  throw std::out_of_range(errStream.str());
}

Memory8::Memory8(const std::size_t memBase) : memLow_(memBase), memory_() {
  memory_.fill(0x0);
  fillTextSprites();
}

void Memory8::fillTextSprites() {
  const std::vector<Byte> sprites = {
      0xF0, // 1111
      0x90, // 1  1
      0x90, // 1  1 -> "0"
      0x90, // 1  1
      0xF0, // 1111

      0x20, //   1
      0x60, //  11
      0x20, //   1 -> "1"
      0x20, //   1
      0x70, //  111

      0xF0, // 1111
      0x10, //    1
      0xF0, // 1111 -> "2"
      0x80, // 1
      0xF0, // 1111

      0xF0, // 1111
      0x10, //    1
      0xF0, // 1111 -> "3"
      0x10, //    1
      0xF0, // 1111

      0x90, // 1  1
      0x90, // 1  1
      0xF0, // 1111 -> "4"
      0x10, //    1
      0x10, //    1

      0xF0, // 1111
      0x80, // 1
      0xF0, // 1111 -> "5"
      0x10, //    1
      0xF0, // 1111

      0xF0, // 1111
      0x80, // 1
      0xF0, // 1111 -> "6"
      0x90, // 1  1
      0xF0, // 1111

      0xF0, // 1111
      0x10, //    1
      0x20, //   1  -> "7"
      0x40, //  1
      0x40, //  1

      0xF0, // 1111
      0x90, // 1  1
      0xF0, // 1111 -> "8"
      0x90, // 1  1
      0xF0, // 1111

      0xF0, // 1111
      0x90, // 1  1
      0xF0, // 1111 -> "9"
      0x10, //    1
      0xF0, // 1111

      0xF0, // 1111
      0x90, // 1  1
      0xF0, // 1111 -> "A"
      0x90, // 1  1
      0x90, // 1  1

      0xE0, // 111
      0x90, // 1  1
      0xE0, // 111  -> "B"
      0x90, // 1  1
      0xE0, // 111

      0xF0, // 1111
      0x80, // 1
      0x80, // 1    -> "C"
      0x80, // 1
      0xF0, // 1111

      0xE0, // 111
      0x90, // 1  1
      0x90, // 1  1 -> "D"
      0x90, // 1  1
      0xE0, // 111

      0xF0, // 1111
      0x80, // 1
      0xF0, // 1111 -> "E"
      0x80, // 1
      0xF0, // 1111

      0xF0, // 1111
      0x80, // 1
      0xF0, // 1111 -> "F"
      0x80, // 1
      0x80  // 1
  };

  std::copy(sprites.begin(), sprites.end(), memory_.begin() + spriteBegin);
}

auto Memory8::fetchInstruction(const Address addr) const -> Instruction {
  if (addr >= memSize - 1) {
    reportInvalidAccess(addr);
  }

  const auto msb = memory_.at(addr);
  const auto lsb = memory_.at(addr + 1);

  return bits8::fuseBytes(msb, lsb);
}

auto Memory8::fetchByte(const Address addr) const -> Byte {
  if (addr >= memSize) {
    reportInvalidAccess(addr);
  }

  return memory_.at(addr);
}

void Memory8::fetchSequence(const Address addr, const Word size,
                            std::vector<Byte> &buf) const {
  if (addr + size > memSize) {
    reportInvalidAccess(addr);
  }

  std::copy(memory_.begin() + addr, memory_.begin() + addr + size,
            std::back_inserter(buf));
}

void Memory8::setByte(const Address addr, Byte val) {
  if (addr >= memSize) {
    reportInvalidAccess(addr);
  }

  memory_[addr] = val;
}

void Memory8::setSequence(const Address addr, const Word size,
                          const std::vector<Byte> &buf) {
  if (addr + size > memSize) {
    reportInvalidAccess(addr);
  }

  std::copy(buf.begin(), buf.end(), memory_.begin() + addr);
}

void Memory8::loadProgram(std::istream &progStream) {
  progStream.unsetf(std::ios::skipws);

  const std::size_t bufSize = memSize - memLow_;
  std::copy_n(std::istream_iterator<Byte>(progStream), bufSize,
              memory_.begin() + memLow_);

  progStream.setf(std::ios::skipws);
}

// FIXME: use in error reporting, add testing flag!
void Memory8::dumpCore(std::ostream &coreStream) const {
  coreStream.unsetf(std::ios::skipws);

  std::copy(memory_.begin() + memLow_, memory_.end(),
            std::ostream_iterator<Byte>(coreStream));

  coreStream.setf(std::ios::skipws);
}
