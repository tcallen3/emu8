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

#ifndef EMU8_INTERFACE_H
#define EMU8_INTERFACE_H

#include <SDL2/SDL.h>
#include <array>
#include <sstream>
#include <string>
#include <vector>

#include "common.h"

class Interface8 {
public:
  static constexpr int fieldWidth = 64;
  static constexpr int fieldHeight = 32;
  static constexpr int defaultScaling = 10;
  static constexpr std::size_t textureSize =
      fieldWidth * fieldHeight / CHAR_BIT;

  explicit Interface8(const std::string &romName, int scaling = defaultScaling);
  ~Interface8();

  // avoid working with SDL move/copy semantics
  Interface8(const Interface8 &other) = delete;
  Interface8(Interface8 &&other) = delete;
  auto operator=(const Interface8 &other) -> Interface8 & = delete;
  auto operator=(Interface8 &&other) -> Interface8 & = delete;

  void ClearScreen();
  auto UpdateScreen(const std::vector<Byte> &newScreen) -> bool;

  void TempTest();

private:
  // set black and white color palette
  const std::array<SDL_Color, 2> colors_ = {
      SDL_Color{0, 0, 0, BYTE_MAX},
      SDL_Color{BYTE_MAX, BYTE_MAX, BYTE_MAX, BYTE_MAX}};

  SDL_Window *window_ = {nullptr};
  SDL_Surface *surface_ = {nullptr};
  SDL_Renderer *renderer_ = {nullptr};
  SDL_Texture *screenTexture_ = {nullptr};

  std::stringstream errStream_ = {};

  int scaling_;
  int screenWidth_;
  int screenHeight_;

  void CreateWindow(const std::string &title);
  void CreateRenderer();
  void CreateSurface();
  void RenderSurface();
};

#endif /* EMU8_INTERFACE_H */
