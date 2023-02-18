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

// FIXME: change method names to lowercase initial letter

static void ReportInvalidAccess(Address addr) {
  std::stringstream errStream;
  errStream << "Invalid memory access: ";
  errStream << std::hex << addr;
  throw std::runtime_error(errStream.str());
}

Memory8::Memory8(const std::size_t memBase) : memLow_(memBase), memory_() {
  memory_.fill(0x0);
}

auto Memory8::FetchInstruction(const Address addr) const -> Instruction {
  if (addr < memLow_ || addr >= memSize - 1) {
    ReportInvalidAccess(addr);
  }

  const auto msb = memory_.at(addr);
  const auto lsb = memory_.at(addr + 1);

  return bits8::fuseBytes(msb, lsb);
}

auto Memory8::FetchByte(const Address addr) const -> Byte {
  if (addr < memLow_ || addr >= memSize) {
    ReportInvalidAccess(addr);
  }

  return memory_.at(addr);
}

void Memory8::FetchSequence(const Address addr, const Word size,
                            std::vector<Byte> &buf) const {
  if (addr < memLow_) {
    ReportInvalidAccess(addr);
  } else if (addr + size > memSize) {
    ReportInvalidAccess(addr + size);
  }

  buf.reserve(size);
  std::copy(memory_.begin() + addr, memory_.begin() + addr + size,
            std::back_inserter(buf));
}

void Memory8::SetByte(const Address addr, Byte val) {
  if (addr < memLow_ || addr >= memSize) {
    ReportInvalidAccess(addr);
  }

  memory_[addr] = val;
}

void Memory8::SetSequence(const Address addr, const Word size,
                          const std::vector<Byte> &buf) {
  if (addr < memLow_) {
    ReportInvalidAccess(addr);
  } else if (addr + size > memSize) {
    ReportInvalidAccess(addr + size);
  }

  std::copy(buf.begin(), buf.end(), memory_.begin() + addr);
}

void Memory8::LoadProgram(std::istream &progStream) {
  progStream.unsetf(std::ios::skipws);

  const std::size_t bufSize = memSize - memLow_;
  std::copy_n(std::istream_iterator<Byte>(progStream), bufSize,
              memory_.begin() + memLow_);

  progStream.setf(std::ios::skipws);
}

void Memory8::DumpCore(std::ostream &coreStream) const {
  coreStream.unsetf(std::ios::skipws);

  std::copy(memory_.begin() + memLow_, memory_.end(),
            std::ostream_iterator<Byte>(coreStream));

  coreStream.setf(std::ios::skipws);
}
