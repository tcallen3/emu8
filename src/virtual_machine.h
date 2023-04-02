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

#ifndef EMU8_VIRTUAL_MACHINE_H
#define EMU8_VIRTUAL_MACHINE_H

#include <SDL2/SDL_scancode.h>
#include <map>
#include <string>

#include "common.h"
#include "instruction_set.h"
#include "interface.h"
#include "memory.h"
#include "register_set.h"

class VirtualMachine8 {
public:
  // set instruction rate around 400 Hz
  static constexpr std::size_t iptDefault = 7;

  struct Settings {
    int scaling{Interface8::defaultScaling};
    Address audioSize{Interface8::defaultAudioBufSize};
    std::size_t memBase{Memory8::loadAddrDefault};
    std::size_t ipt{};
    std::string config{};
    std::string romFile{};
  };

  VirtualMachine8(const std::string &title, const Settings &settings);

  void LoadKeyConfig(const std::string &config);
  auto Run(const std::string &romFile) -> int;

private:
  std::size_t memBase_;
  std::size_t instrPerTick_;
  std::size_t instrCount_{0};

  Interface8 interface_;
  Memory8 memory_;
  RegisterSet8 regSet_ = {};
  InstructionSet8 instructionSet_;

  static auto ParseFile(const std::string &iniFile)
      -> std::map<Byte, SDL_Scancode>;

  void TickReset();
};

#endif /* EMU8_VIRTUAL_MACHINE_H */
