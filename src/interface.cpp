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

#include <stdexcept>

#include "interface.h"

Interface8::Interface8(const std::string &romFile, int scaling)
    : scaling_(scaling), screenWidth_(scaling_ * fieldWidth),
      screenHeight_(scaling_ * fieldHeight) {

  // FIXME: add audio intialization later
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    errStream_ << "SDL initialization failed: ";
    errStream_ << SDL_GetError();
    throw std::runtime_error(errStream_.str());
  }

  // FIXME: migrate app name to variable
  const std::string title = std::string{"emu8 - "} + romFile;
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

// see if we can get something to show
void Interface8::TempTest() {
  const Byte filled = 0xFF;

  int i = 0;
  while (i < fieldWidth * fieldHeight / CHAR_BIT) {
    Byte *pix = static_cast<Byte *>(surface_->pixels);
    pix[i] = filled;

    SDL_RenderClear(renderer_);
    screenTexture_ = SDL_CreateTextureFromSurface(renderer_, surface_);
    SDL_RenderCopy(renderer_, screenTexture_, nullptr, nullptr);
    SDL_RenderPresent(renderer_);
    SDL_DestroyTexture(screenTexture_);

    i++;
  }
}
