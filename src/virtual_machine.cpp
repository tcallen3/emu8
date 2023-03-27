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
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include "virtual_machine.h"

VirtualMachine8::VirtualMachine8(const std::string &title, int displayScale,
                                 std::size_t memBase)
    : memBase_(memBase), interface_(title, displayScale), memory_(memBase),
      instructionSet_(regSet_, memory_, interface_) {}

void VirtualMachine8::LoadKeyConfig(const std::string &config) {
  auto mapping = parser_.ParseFile(config);
  interface_.SetKeyMapping(std::move(mapping));
}

int VirtualMachine8::Run(const std::string &romFile) {
  std::ifstream romData(romFile, std::ios::binary);
  if (!romData.good()) {
    std::cerr << "Could not open ROM file: " << romFile << '\n';
    return EXIT_FAILURE;
  }

  // FIXME: add register decrement and audio output
  try {
    memory_.loadProgram(romData);
    romData.close();

    regSet_.pc = static_cast<Address>(memBase_);

    bool quit = false;
    while (!quit) {
      auto opcode = memory_.fetchInstruction(regSet_.pc);
      instructionSet_.DecodeExecuteInstruction(opcode);
      regSet_.pc += 2;

      if (SDL_HasEvent(SDL_QUIT)) {
        quit = true;
      }
    }
  } catch (const std::exception &err) {
    std::cerr << err.what() << '\n';

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
