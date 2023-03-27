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
#include <vector>

#include <boost/program_options.hpp>

#include "memory.h"
#include "virtual_machine.h"

namespace bpo = boost::program_options;

struct Settings {
  bool etiOn{false};
  int scaling{Interface8::defaultScaling};
  std::string config{};
  std::string romFile{};
};

void usage(const std::string &prog) {
  const std::filesystem::path progPath{prog};
  std::cerr << "usage: " << progPath.filename().string() << " "
            << "[--help] [--config conf.ini] [-s|--scaling scale_factor] "
            << "[--eti660] romfile\n";
}

auto parse_options(int argc, std::vector<char *> &argv, Settings &settings)
    -> bool {
  bpo::options_description visible("Options");
  // clang-format off
  visible.add_options()
    ("config", bpo::value<std::string>(&settings.config), 
     "Keybind config file")
    ("eti660", "Load ROM using ETI 660 address conventions")
    ("help", "Display help message")
    ("scaling,s", bpo::value<int>(&settings.scaling)
                    ->default_value(Interface8::defaultScaling),
     "Video resolution scaling");
  // clang-format on

  bpo::options_description hidden("Hidden options");
  // clang-format off
  hidden.add_options()
    ("inputFile", bpo::value<std::string>(&settings.romFile),
     "Input ROM file");
  // clang-format on

  bpo::options_description cmdlineOptions;
  cmdlineOptions.add(visible).add(hidden);

  bpo::positional_options_description posOpt;
  posOpt.add("inputFile", 1);

  bpo::variables_map varMap;
  bpo::store(bpo::command_line_parser(argc, argv.data())
                 .options(cmdlineOptions)
                 .positional(posOpt)
                 .run(),
             varMap);
  bpo::notify(varMap);

  if (varMap.count("help") != 0) {
    std::cout << visible << '\n';
    return (varMap.count("inputFile") != 0);
  }

  if (varMap.count("eti660") != 0) {
    settings.etiOn = true;
  }

  return (varMap.count("inputFile") != 0);
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

  std::cout << '\n';
}

auto main(int argc, char *argv[]) -> int {
  std::vector<char *> vecArgs(argv, argv + argc);

  Settings progSettings;
  bool inputPresent{false};
  try {
    inputPresent = parse_options(argc, vecArgs, progSettings);
  } catch (const std::exception &err) {
    std::cerr << err.what() << '\n';
    usage(vecArgs.front());
    return EXIT_FAILURE;
  }

  if (!inputPresent) {
    usage(vecArgs.front());
    return EXIT_FAILURE;
  }

  print_license();

  const std::filesystem::path title{progSettings.romFile};
  auto memBase =
      (progSettings.etiOn) ? Memory8::loadAddrEti660 : Memory8::loadAddrDefault;

  VirtualMachine8 vm8(title, progSettings.scaling, memBase);

  if (!progSettings.config.empty()) {
    try {
      vm8.LoadKeyConfig(progSettings.config);
    } catch (const std::runtime_error &err) {
      std::cerr << "Error loading keybinding config file: " << err.what()
                << '\n';
      return EXIT_FAILURE;
    }
  }

  auto retval = vm8.Run(progSettings.romFile);

  return retval;
}
