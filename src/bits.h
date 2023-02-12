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

using BytePair = std::pair<Byte, Byte>;

namespace bits8 {

constexpr Word LOW_BYTE_ON = 0xFF;

/* 
 * combine 8-bit msb and 8-bit lsb into single 16-bit word 
 * as (msb,lsb)
 */
constexpr Word fuseBytes(Byte msb, Byte lsb)
{
  Word high = msb;
  Word low = lsb;

  return (high << BITS_IN_BYTE) | low;
}

/*
 * perform inverse of fuseBytes(), splitting a single 16-bit word
 * into its most-significant and least-significant bytes, returning
 * them as a pair in the order (msb, lsb)
 */
constexpr BytePair splitWord(Word word)
{
  Byte low = static_cast<Byte>(word | LOW_BYTE_ON);
  Byte high = static_cast<Byte>((word >> BITS_IN_BYTE) | LOW_BYTE_ON);

  return {low, high};
}

} /* bits8 */

#endif /* EMU8_BITS_H */
