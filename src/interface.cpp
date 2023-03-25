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
#include <cassert>
#include <stdexcept>

#include "interface.h"

Interface8::Interface8(const std::string &romName, int scaling)
    : scaling_(scaling), screenWidth_(scaling_ * fieldWidth),
      screenHeight_(scaling_ * fieldHeight) {

  // FIXME: add audio intialization later
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    errStream_ << "SDL initialization failed: ";
    errStream_ << SDL_GetError();
    throw std::runtime_error(errStream_.str());
  }

  // FIXME: migrate app name to variable
  const std::string title = std::string{"emu8 - "} + romName;
  CreateWindow(title);
  CreateRenderer();
  CreateSurface();
}

void Interface8::CreateWindow(const std::string &title) {
  window_ =
      SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, screenWidth_, screenHeight_, 0);

  if (window_ == nullptr) {
    errStream_ << "SDL window initialization failed: ";
    errStream_ << SDL_GetError();
    throw std::runtime_error(errStream_.str());
  }

  SDL_SetWindowMinimumSize(window_, screenWidth_, screenHeight_);
}

void Interface8::CreateRenderer() {
  renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_PRESENTVSYNC);
  if (renderer_ == nullptr) {
    errStream_ << "SDL renderer initialization failed: ";
    errStream_ << SDL_GetError();
    throw std::runtime_error(errStream_.str());
  }

  if (SDL_RenderSetLogicalSize(renderer_, screenWidth_, screenHeight_) < 0) {
    errStream_ << "SDL renderer logical sizing error: ";
    errStream_ << SDL_GetError();
    throw std::runtime_error(errStream_.str());
  }

  // if window would be between integer sizes, scale down to the smaller value
  if (SDL_RenderSetIntegerScale(renderer_, SDL_TRUE) < 0) {
    errStream_ << "SDL renderer integer scaling error: ";
    errStream_ << SDL_GetError();
    throw std::runtime_error(errStream_.str());
  }

  const auto fscale = static_cast<float>(scaling_);
  if (SDL_RenderSetScale(renderer_, fscale, fscale) < 0) {
    errStream_ << "SDL renderer scaling error: ";
    errStream_ << SDL_GetError();
    throw std::runtime_error(errStream_.str());
  }
}

void Interface8::CreateSurface() {
  surface_ = SDL_CreateRGBSurfaceWithFormat(0, fieldWidth, fieldHeight, 1,
                                            SDL_PIXELFORMAT_INDEX1MSB);

  if (surface_ == nullptr) {
    errStream_ << "SDL surface initialization error: ";
    errStream_ << SDL_GetError();
    throw std::runtime_error(errStream_.str());
  }

  if (SDL_SetPaletteColors(surface_->format->palette, colors_.data(), 0,
                           static_cast<int>(colors_.size())) < 0) {
    errStream_ << "SDL surface palette error: ";
    errStream_ << SDL_GetError();
    throw std::runtime_error(errStream_.str());
  }
}

Interface8::~Interface8() {
  if (screenTexture_ != nullptr) {
    SDL_DestroyTexture(screenTexture_);
  }

  SDL_FreeSurface(surface_);

  if (renderer_ != nullptr) {
    SDL_DestroyRenderer(renderer_);
  }

  if (window_ != nullptr) {
    SDL_DestroyWindow(window_);
  }

  SDL_Quit();
}

void Interface8::RenderSurface() {
  if (SDL_RenderClear(renderer_) < 0) {
    errStream_ << "Error clearing renderer: ";
    errStream_ << SDL_GetError();
    throw std::runtime_error(errStream_.str());
  }

  screenTexture_ = SDL_CreateTextureFromSurface(renderer_, surface_);
  if (screenTexture_ == nullptr) {
    errStream_ << "Could not create screen texture: ";
    errStream_ << SDL_GetError();
    throw std::runtime_error(errStream_.str());
  }

  if (SDL_RenderCopy(renderer_, screenTexture_, nullptr, nullptr) < 0) {
    errStream_ << "Error in render copy: ";
    errStream_ << SDL_GetError();
    throw std::runtime_error(errStream_.str());
  }

  SDL_RenderPresent(renderer_);
  SDL_DestroyTexture(screenTexture_);
}

void Interface8::ClearScreen() {
  std::vector<Byte> bitVec(textureSize, 0);
  Byte *pix = static_cast<Byte *>(surface_->pixels);
  std::copy(bitVec.begin(), bitVec.end(), pix);

  RenderSurface();
}

auto Interface8::UpdateScreen(const std::vector<Byte> &newScreen) -> bool {
  assert(newScreen.size() == textureSize);

  std::vector<Byte> currScreen;
  Byte *pix = static_cast<Byte *>(surface_->pixels);
  std::copy_n(pix, textureSize, std::back_inserter(currScreen));

  bool flipped = false;
  std::vector<Byte> finalScreen(textureSize, 0);
  for (std::size_t index = 0; index < newScreen.size(); index++) {
    finalScreen[index] = currScreen[index] ^ newScreen[index];

    // bitwise check that we haven't flipped off pixels, look at truth table
    // for XOR plus AND to confirm this is correct
    if ((currScreen[index] & finalScreen[index]) != currScreen[index]) {
      flipped = true;
    }
  }

  std::copy(finalScreen.begin(), finalScreen.end(), pix);
  RenderSurface();

  return flipped;
}

// see if we can get something to show
void Interface8::TempTest() {
  const Byte filled = 0xFF;

  std::size_t i = 0;
  while (i < textureSize) {
    std::vector<Byte> bitVec(textureSize, 0);
    bitVec[i] = filled;
    Byte *pix = static_cast<Byte *>(surface_->pixels);

    std::copy(bitVec.begin(), bitVec.end(), pix);
    RenderSurface();

    i++;
  }
}
