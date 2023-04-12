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

#include <SDL2/SDL.h>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>

#include "virtual_machine.h"

namespace bpt = boost::property_tree;

VirtualMachine8::VirtualMachine8(const std::string &title,
                                 const Settings &settings)
    : memBase_(settings.memBase), instrPerTick_(settings.ipt),
      interface_(title, regSet_, settings.audioSize, settings.scaling),
      memory_(settings.memBase), instructionSet_(regSet_, memory_, interface_) {
  if (!settings.config.empty()) {
    LoadKeyConfig(settings.config);
  }
}

auto VirtualMachine8::ParseFile(const std::string &iniFile)
    -> std::map<Byte, SDL_Scancode> {
  const std::string section = "keybindings";
  const std::vector<std::string> keys = {
      "KEY_0", "KEY_1", "KEY_2", "KEY_3", "KEY_4", "KEY_5", "KEY_6", "KEY_7",
      "KEY_8", "KEY_9", "KEY_A", "KEY_B", "KEY_C", "KEY_D", "KEY_E", "KEY_F"};

  bpt::ptree tree;
  bpt::read_ini(iniFile, tree);

  Byte index = 0;
  std::map<Byte, SDL_Scancode> codeMap;

  for (const auto &key : keys) {
    const std::string entry = section + "." + key; // NOLINT
    const auto codeName = tree.get<std::string>(entry);
    const SDL_Scancode scanCode = SDL_GetScancodeFromName(codeName.c_str());

    if (scanCode == SDL_SCANCODE_UNKNOWN) {
      std::string msg = "Unrecognized key scancode name \"";
      msg += codeName + "\": ";
      throw std::runtime_error(msg + SDL_GetError());
    }

    codeMap.insert(std::pair{index, scanCode});
    index++;
  }

  return codeMap;
}

void VirtualMachine8::LoadKeyConfig(const std::string &config) {
  auto mapping = ParseFile(config);
  interface_.SetKeyMapping(std::move(mapping));
}

static auto GetNextTick() {
  using std::chrono::operator""ms;
  const auto tickLen = 16ms;
  return (std::chrono::steady_clock::now() + tickLen);
}

void VirtualMachine8::TickReset() {
  // decrement tick registers
  if (regSet_.regST > 0) {
    regSet_.regST--;
  }
  regSet_.audioOn = (regSet_.regST > 0);

  if (regSet_.regDT > 0) {
    regSet_.regDT--;
  }

  // reset instruction count
  instrCount_ = 0;
}

auto VirtualMachine8::Run(const std::string &romFile) -> int {
  std::ifstream romData(romFile, std::ios::binary);
  if (!romData.good()) {
    std::cerr << "Could not open ROM file: " << romFile << '\n';
    return EXIT_FAILURE;
  }

  try {
    memory_.loadProgram(romData);
    romData.close();

    regSet_.pc = static_cast<Address>(memBase_);
    auto nextTick = GetNextTick();
    instrCount_ = 0;

    bool quit = false;
    while (!quit) {
      SDL_Event event;
      while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
          quit = true;
        }
      }

      if (quit) {
        break;
      }

      if (std::chrono::steady_clock::now() >= nextTick) {
        TickReset();
        nextTick = GetNextTick();
      }

      if (instrCount_ >= instrPerTick_) {
        std::this_thread::sleep_until(nextTick);
        TickReset();
        nextTick = GetNextTick();
      }

      auto opcode = memory_.fetchInstruction(regSet_.pc);
      regSet_.pc += 2;

      instructionSet_.DecodeExecuteInstruction(opcode);
      instrCount_++;
    }
  } catch (const std::exception &err) {
    std::cerr << "ERROR: " << err.what() << '\n';

    std::string coreName = romFile + ".core";
    std::ofstream coreFile(coreName, std::ios::binary);
    if (!coreFile.good()) {
      // nothing else we can do
      return EXIT_FAILURE;
    }

    std::cerr << "Dumping memory core file: " << coreName << '\n';
    memory_.dumpCore(coreFile);
    coreFile.close();
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
