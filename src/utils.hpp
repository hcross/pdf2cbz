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

#pragma once

#include <cmath>
#include <iomanip>
#include <iostream>
#include <mutex>

class ProgressBar {
public:
  ProgressBar() : m_progress(0) {}

  void update(int current, int total) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_progress = current;
    float percent = (float)current / total;
    int barWidth = 50;

    std::cout << "\r[";
    int pos = barWidth * percent;
    for (int i = 0; i < barWidth; ++i) {
      if (i < pos)
        std::cout << "=";
      else if (i == pos)
        std::cout << ">";
      else
        std::cout << " ";
    }
    std::cout << "] " << int(percent * 100.0) << " %" << std::flush;

    if (current == total) {
      std::cout << std::endl;
    }
  }

private:
  std::mutex m_mutex;
  int m_progress;
};
