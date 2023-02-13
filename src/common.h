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

#ifndef EMU8_COMMON_H
#define EMU8_COMMON_H

#include <cstddef>
#include <cstdint>
#include <limits>

using Address = uint16_t;
using Word = uint16_t;
using Byte = uint8_t;

using BytePair = std::pair<Byte, Byte>;

static constexpr std::size_t BITS_IN_BYTE = 8;

static constexpr Address ADDRESS_MIN = std::numeric_limits<Address>::min();
static constexpr Address ADDRESS_MAX = std::numeric_limits<Address>::max();

static constexpr Word WORD_MIN = std::numeric_limits<Word>::min();
static constexpr Word WORD_MAX = std::numeric_limits<Word>::max();

static constexpr Byte BYTE_MIN = std::numeric_limits<Byte>::min();
static constexpr Byte BYTE_MAX = std::numeric_limits<Byte>::max();

#endif /* EMU8_COMMON_H */
