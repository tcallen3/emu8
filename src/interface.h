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
#include <SDL2/SDL_scancode.h>
#include <array>
#include <atomic>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "common.h"
#include "register_set.h"

class Interface8 {
public:
  // video settings
  static constexpr int fieldWidth = 64;
  static constexpr int fieldHeight = 32;
  static constexpr int defaultScaling = 10;
  static constexpr std::size_t textureSize =
      fieldWidth * fieldHeight / CHAR_BIT;

  static constexpr int audioSampleFreq = 44100;
  static constexpr int defaultAudioBufSize = 4096;
  static constexpr int toneFreq = 440;

  // keyboard settings
  static constexpr Byte keyMax = 0xF;

  enum LogicalKeys : Byte {
    CHIP8_KEY_0 = 0x0,
    CHIP8_KEY_1,
    CHIP8_KEY_2,
    CHIP8_KEY_3,
    CHIP8_KEY_4,
    CHIP8_KEY_5,
    CHIP8_KEY_6,
    CHIP8_KEY_7,
    CHIP8_KEY_8,
    CHIP8_KEY_9,
    CHIP8_KEY_A,
    CHIP8_KEY_B,
    CHIP8_KEY_C,
    CHIP8_KEY_D,
    CHIP8_KEY_E,
    CHIP8_KEY_F
  };

  explicit Interface8(const std::string &title, RegisterSet8 &regSet,
                      Address audioSize = defaultAudioBufSize,
                      int scaling = defaultScaling);
  ~Interface8();

  // avoid working with SDL move/copy semantics
  Interface8(const Interface8 &other) = delete;
  Interface8(Interface8 &&other) = delete;
  auto operator=(const Interface8 &other) -> Interface8 & = delete;
  auto operator=(Interface8 &&other) -> Interface8 & = delete;

  void ClearScreen();
  auto UpdateScreen(const std::vector<Byte> &newScreen) -> bool;
  auto KeyPressed(Byte keyVal) -> bool;
  auto GetKeyPress() -> Byte;
  auto SetKeyMapping(std::map<Byte, SDL_Scancode> &&mapping) {
    keyboardMapping_ = mapping;
  }

private:
  // set black and white color palette
  const std::array<SDL_Color, 2> colors_ = {
      SDL_Color{0, 0, 0, BYTE_MAX},
      SDL_Color{BYTE_MAX, BYTE_MAX, BYTE_MAX, BYTE_MAX}};

  // default keyboard mapping, editable via config file
  std::map<Byte, SDL_Scancode> keyboardMapping_ = {
      // first row
      {CHIP8_KEY_1, SDL_SCANCODE_1},
      {CHIP8_KEY_2, SDL_SCANCODE_2},
      {CHIP8_KEY_3, SDL_SCANCODE_3},
      {CHIP8_KEY_C, SDL_SCANCODE_4},

      // second row
      {CHIP8_KEY_4, SDL_SCANCODE_Q},
      {CHIP8_KEY_5, SDL_SCANCODE_W},
      {CHIP8_KEY_6, SDL_SCANCODE_E},
      {CHIP8_KEY_D, SDL_SCANCODE_R},

      // third row
      {CHIP8_KEY_7, SDL_SCANCODE_A},
      {CHIP8_KEY_8, SDL_SCANCODE_S},
      {CHIP8_KEY_9, SDL_SCANCODE_D},
      {CHIP8_KEY_E, SDL_SCANCODE_F},

      // fourth row
      {CHIP8_KEY_A, SDL_SCANCODE_Z},
      {CHIP8_KEY_0, SDL_SCANCODE_X},
      {CHIP8_KEY_B, SDL_SCANCODE_C},
      {CHIP8_KEY_F, SDL_SCANCODE_V}};

  // holds inverse of keyboardMapping
  std::map<SDL_Scancode, Byte> scancodeMapping_ = {};

  SDL_Window *window_ = {nullptr};
  SDL_Surface *surface_ = {nullptr};
  SDL_Renderer *renderer_ = {nullptr};
  SDL_Texture *screenTexture_ = {nullptr};

  std::stringstream errStream_ = {};

  SDL_AudioSpec audioSpec_ = {};
  SDL_AudioDeviceID audioID_ = {};

  int scaling_;
  int screenWidth_;
  int screenHeight_;
  Address audioBufSize_;

  RegisterSet8 &regSet_;

  void CreateWindow(const std::string &title);
  void CreateRenderer();
  void CreateSurface();
  void FillScancodeMap();
  void InitAudio();
  auto ValidKeyPress(const SDL_Event &event) -> bool;
  void RenderSurface();
};

#endif /* EMU8_INTERFACE_H */
