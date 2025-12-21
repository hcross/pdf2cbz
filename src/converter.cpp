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

#include "converter.hpp"
#include "utils.hpp"
#include <algorithm>
#include <atomic>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <poppler-document.h>
#include <poppler-image.h>
#include <poppler-page-renderer.h>
#include <poppler-page.h>
#include <sstream>
#include <thread>
#include <vector>

// Miniz implementation
#include "vendor/miniz.h"

namespace fs = std::filesystem;

Converter::Converter(const std::string &inputPath,
                     const std::string &outputPath)
    : m_inputPath(inputPath), m_outputPath(outputPath) {}

bool Converter::process(int threads) {
  int numThreads = threads;
  if (numThreads <= 0) {
    numThreads = std::thread::hardware_concurrency();
    if (numThreads == 0)
      numThreads = 1; // Fallback
  }

  // Clamp to a sane maximum to avoid system exhaustion
  if (numThreads > 32)
    numThreads = 32;

  std::cout << "Loading PDF for page count: " << m_inputPath << std::endl;
  std::unique_ptr<poppler::document> doc(
      poppler::document::load_from_file(m_inputPath));
  if (!doc) {
    std::cerr << "Error: Could not load PDF file." << std::endl;
    return false;
  }
  if (doc->is_locked()) {
    std::cerr << "Error: PDF is locked." << std::endl;
    return false;
  }
  int totalPages = doc->pages();
  doc.reset();

  std::cout << "Total pages: " << totalPages << std::endl;
  std::cout << "Starting conversion with " << numThreads << " threads..."
            << std::endl;

  mz_zip_archive zip_archive;
  memset(&zip_archive, 0, sizeof(zip_archive));

  if (!mz_zip_writer_init_file(&zip_archive, m_outputPath.c_str(), 0)) {
    std::cerr << "Error: Could not create ZIP file " << m_outputPath
              << std::endl;
    return false;
  }

  std::atomic<int> nextPage{0};
  std::mutex zipMutex;
  std::vector<std::thread> workers;
  std::atomic<bool> success{true};

  ProgressBar bar;
  for (int t = 0; t < numThreads; ++t) {
    workers.emplace_back([&, t]() {
      std::unique_ptr<poppler::document> localDoc(
          poppler::document::load_from_file(m_inputPath));
      if (!localDoc) {
        success = false;
        return;
      }

      poppler::page_renderer renderer;
      renderer.set_render_hint(poppler::page_renderer::antialiasing, true);
      renderer.set_render_hint(poppler::page_renderer::text_antialiasing, true);

      while (true) {
        int i = nextPage.fetch_add(1);
        if (i >= totalPages)
          break;

        if (!success)
          break;

        poppler::page *p = localDoc->create_page(i);
        if (!p) {
          std::cerr << "Warning: Could not create page " << i << std::endl;
          continue;
        }

        poppler::image img = renderer.render_page(p, 150.0, 150.0);
        delete p;

        if (!img.is_valid()) {
          std::cerr << "Warning: Could not render page " << i << std::endl;
          continue;
        }

        std::stringstream ss;
        ss << std::setfill('0') << std::setw(4) << i << ".jpg";
        std::string filename = ss.str();

        std::stringstream temp_ss;
        temp_ss << "temp_" << t << "_" << i << ".jpg";
        std::string temp_file = temp_ss.str();

        if (!img.save(temp_file, "jpeg", 80)) {
          if (!img.save(temp_file, "png")) {
            std::cerr << "Warning: Could not save image for page " << i
                      << std::endl;
            continue;
          }
        }

        {
          std::lock_guard<std::mutex> lock(zipMutex);
          if (!mz_zip_writer_add_file(&zip_archive, filename.c_str(),
                                      temp_file.c_str(), NULL, 0,
                                      MZ_BEST_COMPRESSION)) {
            std::cerr << "Error adding file to zip: " << filename << std::endl;
            success = false;
          }
          // Update progress bar
          static std::atomic<int> completed{0};
          bar.update(++completed, totalPages);
        }

        fs::remove(temp_file);
      }
    });
  }

  for (auto &w : workers) {
    if (w.joinable())
      w.join();
  }

  mz_zip_writer_finalize_archive(&zip_archive);
  mz_zip_writer_end(&zip_archive);

  if (success) {
    std::cout << "Successfully created " << m_outputPath << std::endl;
    return true;
  } else {
    std::cerr << "Conversion failed." << std::endl;
    return false;
  }
}
