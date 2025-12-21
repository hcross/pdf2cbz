#include "converter.hpp"
#include "version.hpp"
/*
    pdf2cbz - PDF to CBZ Converter
    Copyright (C) 2025 Hoani Cross

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <string>

void printUsage(const char *progName) {
  std::cout << "Usage: " << progName
            << " <input.pdf> [output.cbz] [--threads/-t <N>] [--version|-v]"
            << std::endl;
}

void printVersion() {
  std::cout << "pdf2cbz version " << PROJECT_VERSION << std::endl;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printUsage(argv[0]);
    return 1;
  }

  std::string inputPath;
  std::string outputPath;
  int threads = 0; // 0 means default/auto

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "--threads" || arg == "-t") {
      if (i + 1 < argc) {
        try {
          threads = std::stoi(argv[++i]);
        } catch (...) {
          std::cerr << "Invalid thread count." << std::endl;
          return 1;
        }
      } else {
        std::cerr << "Missing thread count argument." << std::endl;
        return 1;
      }
    } else if (arg == "--version" || arg == "-v") {
      printVersion();
      return 0;
    } else if (arg == "--parallel" || arg == "-p") {
      // Deprecated but ignore
      std::cout
          << "Warning: --parallel/-p is deprecated. Parallel is now default."
          << std::endl;
    } else if (inputPath.empty()) {
      inputPath = arg;
    } else if (outputPath.empty()) {
      outputPath = arg;
    }
  }

  if (inputPath.empty()) {
    printUsage(argv[0]);
    return 1;
  }

  if (outputPath.empty()) {
    size_t dotPos = inputPath.find_last_of('.');
    if (dotPos != std::string::npos) {
      outputPath = inputPath.substr(0, dotPos) + ".cbz";
    } else {
      outputPath = inputPath + ".cbz";
    }
  }

  std::cout << "Converting " << inputPath << " -> " << outputPath << std::endl;

  Converter converter(inputPath, outputPath);
  if (converter.process(threads)) {
    std::cout << "Done!" << std::endl;
    return 0;
  } else {
    std::cerr << "Conversion failed." << std::endl;
    return 1;
  }
}
