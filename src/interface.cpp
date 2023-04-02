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
#include <cmath>
#include <stdexcept>

#include "interface.h"

static void AudioCB(void *userdata, Uint8 *stream, int len) {
  const auto twoPi = static_cast<float>(2 * std::acos(-1.0F));
  const float amplitude = 0.1F;
  const auto sampleFreq = static_cast<float>(Interface8::audioSampleFreq);
  const auto toneFreq = static_cast<float>(Interface8::toneFreq);

  static float phase = 0.0;

  // we have to reinterpret here since SDL is stuck on C conventions
  auto *regPtr = reinterpret_cast<RegisterSet8 *>(userdata); // NOLINT
  if (!regPtr->audioOn) {
    // can use len directly since it measures size of stream in bytes
    SDL_memset(stream, 0, len);
    return;
  }

  // we have to reinterpret here since SDL is stuck on C conventions
  auto *buf = reinterpret_cast<float *>(stream); // NOLINT

  // we assume only a single mono channel in the stream
  const int max = len / static_cast<int>(sizeof(float) / sizeof(Uint8));

  int idx = 0;
  for (; idx < max; idx++) {
    auto sinVal = std::sin(
        twoPi * (toneFreq / sampleFreq) * static_cast<float>(idx) + phase);
    buf[idx] = amplitude * static_cast<float>(sinVal); // NOLINT
  }
  phase = twoPi * (toneFreq / sampleFreq) * static_cast<float>(idx);
}

Interface8::Interface8(const std::string &title, RegisterSet8 &regSet,
                       Address audioSize, int scaling)
    : scaling_(scaling), screenWidth_(scaling_ * fieldWidth),
      screenHeight_(scaling_ * fieldHeight), audioBufSize_(audioSize),
      regSet_(regSet) {

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
    errStream_ << "SDL initialization failed: ";
    errStream_ << SDL_GetError();
    throw std::runtime_error(errStream_.str());
  }

  const std::string header = machineName + " - " + title;
  CreateWindow(header);
  CreateRenderer();
  CreateSurface();
  FillScancodeMap();
  InitAudio();
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

void Interface8::FillScancodeMap() {
  scancodeMapping_.clear();
  for (const auto &[keyVal, scanCode] : keyboardMapping_) {
    scancodeMapping_.insert(std::pair{scanCode, keyVal});
  }
}

void Interface8::InitAudio() {
  SDL_AudioSpec requested;
  SDL_memset(&requested, 0, sizeof(requested));

  requested.freq = audioSampleFreq;
  requested.format = AUDIO_F32SYS;
  requested.channels = 1;
  requested.samples = audioBufSize_;
  requested.callback = AudioCB;
  requested.userdata = &regSet_;

  audioID_ = SDL_OpenAudioDevice(nullptr, 0, &requested, &audioSpec_, 0);
  if (audioID_ == 0) {
    errStream_ << "Failed to open audio device: ";
    errStream_ << SDL_GetError();
    throw std::runtime_error(errStream_.str());
  }

  // unpause the audio stream
  SDL_PauseAudioDevice(audioID_, 0);
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

  SDL_CloseAudioDevice(audioID_);
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

auto Interface8::KeyPressed(Byte keyVal) -> bool {
  auto scanCode = keyboardMapping_.at(keyVal);

  int size = 0;
  const auto *keyArray = SDL_GetKeyboardState(&size);
  assert(scanCode < size);

  return (keyArray[scanCode] == 1); // NOLINT
}

// helper function to clarify keypress logic
auto Interface8::ValidKeyPress(const SDL_Event &event) -> bool {
  // must be key press event
  if (event.type != SDL_KEYDOWN) {
    return false;
  }

  // key scan code must represent a known Chip-8 key
  return (scancodeMapping_.count(event.key.keysym.scancode) == 1);
}

auto Interface8::GetKeyPress() -> Byte {
  SDL_Event event;
  do {
    if (SDL_WaitEvent(&event) == 0) {
      const std::string msg = "Error waiting for SDL key press event: ";
      throw std::runtime_error(msg + SDL_GetError());
    }
  } while (!ValidKeyPress(event));

  return scancodeMapping_.at(event.key.keysym.scancode);
}
