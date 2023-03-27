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

#ifndef EMU8_CONFIG_PARSER_H
#define EMU8_CONFIG_PARSER_H

#include <SDL2/SDL_scancode.h>
#include <map>
#include <string>

#include "common.h"

class ConfigParser {
public:
  ConfigParser() = default;

  static auto ParseFile(const std::string &iniFile)
      -> std::map<Byte, SDL_Scancode>;
};

#endif /* EMU8_CONFIG_PARSER_H */
