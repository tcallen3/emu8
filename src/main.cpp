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

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

#include "interface.h"
#include "memory.h"

struct Settings {
  bool etiOn{false};
  int scaling{Interface8::defaultScaling};
  std::string config{};
  std::string romFile{};
};

void usage(const std::string &prog) {
  const std::filesystem::path progPath{prog};
  std::cerr << "usage: " << progPath.filename() << " "
            << "[--help] [--config conf.ini] [-s scale_factor] [--eti660] "
            << "romfile\n";

  std::cerr << "\tUse '--help' for more information\n";
}

void parse_options(int ac, char *av[], Settings &settings) {
  // FIXME: implement
  std::ignore = ac;
  std::ignore = av;
  settings.romFile = av[1];
}

void print_license() {
  std::cout << "------------------------------------\n"
            << "emu8 Copyright (C) 2023 Thomas Allen\n"
            << "------------------------------------\n";

  std::cout << "This program comes with ABSOLUTELY NO WARRANTY. "
            << "It is free software, and you are welcome to redistribute it "
            << "under certain conditions. ";

  std::cout << "For more information, see the GNU General Public License "
            << "<https://www.gnu.org/licenses>.\n";
}

auto main(int argc, char *argv[]) -> int {

  if (argc < 2) {
    usage(argv[0]);
    return EXIT_FAILURE;
  }

  Settings progSettings;
  try {
    parse_options(argc, argv, progSettings);
  } catch (const std::exception &err) {
    std::cerr << err.what() << '\n';
    std::cerr << '\n';

    usage(argv[0]);
    return EXIT_FAILURE;
  }

  print_license();

  Memory8 mem(Memory8::loadAddrDefault);

  const std::filesystem::path title{progSettings.romFile};
  Interface8 interface(title.stem(), progSettings.scaling);

  interface.TempTest();

  return EXIT_SUCCESS;
}
