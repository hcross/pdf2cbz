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
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <poppler-document.h>
#include <poppler-image.h>
#include <poppler-page-renderer.h>
#include <poppler-page.h>
#include <sstream>
#include <vector>

// Miniz implementation
#include "vendor/miniz.h"

namespace fs = std::filesystem;

Converter::Converter(const std::string &inputPath,
                     const std::string &outputPath)
    : m_inputPath(inputPath), m_outputPath(outputPath) {}

bool Converter::process() {
  std::cout << "Loading PDF: " << m_inputPath << std::endl;

  // Load PDF document
  poppler::document *doc = poppler::document::load_from_file(m_inputPath);
  if (!doc) {
    std::cerr << "Error: Could not load PDF file." << std::endl;
    return false;
  }

  if (doc->is_locked()) {
    std::cerr << "Error: PDF is locked." << std::endl;
    delete doc;
    return false;
  }

  int pages = doc->pages();
  std::cout << "Total pages: " << pages << std::endl;

  // Initialize ZIP archive
  mz_zip_archive zip_archive;
  memset(&zip_archive, 0, sizeof(zip_archive));

  if (!mz_zip_writer_init_file(&zip_archive, m_outputPath.c_str(), 0)) {
    std::cerr << "Error: Could not create ZIP file " << m_outputPath
              << std::endl;
    delete doc;
    return false;
  }

  poppler::page_renderer renderer;
  renderer.set_render_hint(poppler::page_renderer::antialiasing, true);
  renderer.set_render_hint(poppler::page_renderer::text_antialiasing, true);

  for (int i = 0; i < pages; ++i) {
    std::cout << "Processing page " << (i + 1) << "/" << pages << "..."
              << std::endl;
    poppler::page *p = doc->create_page(i);
    if (!p) {
      std::cerr << "Warning: Could not create page " << i << std::endl;
      continue;
    }

    // Render page to image
    // 150 DPI is usually good for comics
    poppler::image img = renderer.render_page(p, 150.0, 150.0);

    if (!img.is_valid()) {
      std::cerr << "Warning: Could not render page " << i << std::endl;
      delete p;
      continue;
    }

    // Convert to logic that miniz likes or just dump raw bytes if we had a easy
    // way. Poppler gives us raw BGRA or RGB data. We need to encode this as
    // JPEG or PNG to put inside the CBZ. A simple way is to use a STB image
    // writer or similar, BUT requested task is "only C++" (and deps I
    // gathered). Poppler might support saving to file? poppler::image has save
    // method!

    std::stringstream ss;
    ss << std::setfill('0') << std::setw(4) << i << ".jpg";
    std::string filename = ss.str();

    // Warning: poppler::image::save wants a filename, not a buffer.
    // So we have to write to a temp file then read it back into the zip.
    // This is a bit inefficient but easy to implement.

    std::string temp_file = "temp_" + filename;
    if (!img.save(temp_file, "jpeg", 80)) { // 80 quality jpeg
      // Fallback to png if jpeg not supported by specific poppler build?
      // Usually it supports "jpeg", "png", "tiff".
      if (!img.save(temp_file, "png")) {
        std::cerr << "Warning: Could not save image for page " << i
                  << std::endl;
        delete p;
        continue;
      }
    }

    // Read file content
    // In a real optimized app we would avoid disk IO but this is a CLI tool.
    // Wait, miniz has mz_zip_writer_add_file which reads from disk!

    if (!mz_zip_writer_add_file(&zip_archive, filename.c_str(),
                                temp_file.c_str(), NULL, 0,
                                MZ_BEST_COMPRESSION)) {
      std::cerr << "Error adding file to zip: " << filename << std::endl;
    }

    // cleanup temp file
    fs::remove(temp_file);
    delete p;
  }

  mz_zip_writer_finalize_archive(&zip_archive);
  mz_zip_writer_end(&zip_archive);
  delete doc;

  std::cout << "Successfully created " << m_outputPath << std::endl;
  return true;
}
