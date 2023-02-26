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

#ifndef EMU8_BITS_H
#define EMU8_BITS_H

#include <utility>

#include "common.h"

namespace bits8 {

constexpr Word LOW_BYTE_ON = 0xFF;

/* 
 * combine 8-bit msb and 8-bit lsb into single 16-bit word 
 * as (msb,lsb)
 */
constexpr auto fuseBytes(Byte msb, Byte lsb) -> Word {
  Word high = msb;
  Word low = lsb;

  return (high << CHAR_BIT) | low;
}

/*
 * perform inverse of fuseBytes(), splitting a single 16-bit word
 * into its most-significant and least-significant bytes, returning
 * them as a pair in the order (msb, lsb)
 */
constexpr auto splitWord(Word word) -> BytePair {
  Byte low = static_cast<Byte>(word & LOW_BYTE_ON);
  Byte high = static_cast<Byte>((word >> CHAR_BIT) & LOW_BYTE_ON);

  return {high, low};
}

/*
 * given a two-byte instruction, mask out the lower 12 bits
 * to get a valid Chip-8 memory address
 */
constexpr auto maskAddress(Instruction opcode) -> Address {
  constexpr Address mask = 0x0FFF;
  return opcode & mask;
}

/*
 * mask out and return the lowest 4 bits from a given byte
 */
constexpr auto lowNibble(Byte val) -> Byte {
  constexpr Byte mask = 0x0F;
  return val & mask;
}

/*
 * return the highest 4 bits of a given byte
 */
constexpr auto highNibble(Byte val) -> Byte {
  constexpr Byte nibSize = 4;
  return val >> nibSize;
}

/*
 * return the least significant bit of a given byte
 */
auto inline getLsb(Byte val) -> Byte { return (val & 0x1); }

/*
 * return the most significant bit of a given byte
 */
auto inline getMsb(Byte val) -> Byte {
  const Byte mask = 0x80;
  return ((val & mask) != 0) ? 1 : 0;
}

} // namespace bits8

#endif /* EMU8_BITS_H */
