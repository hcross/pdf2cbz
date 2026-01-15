# AGENTS.md

This document provides a technical overview of the `pdf2cbz` project for developers and AI agents. It describes the architecture, CI/CD pipelines, and the standard workflow for contributions.

## Project Overview

`pdf2cbz` is a high-performance C++ CLI tool designed to convert PDF documents into CBZ (Comic Book Archive) files. It leverages parallel processing to speed up the rendering and compression phases.

## Architecture

The project follows a standard C++ structure and uses CMake as its build system.

### Key Components

- **`src/main.cpp`**: Handles command-line interface (CLI) arguments using a simple flag-based approach. It manages the orchestration of the conversion process.
- **`src/converter.cpp`**: Contains the core logic for PDF-to-CBZ conversion. It utilizes Poppler to render PDF pages into images and then packages them into a ZIP archive (CBZ).
- **`src/utils.hpp`**: Provides utility functions for file system operations and image handling.
- **`src/vendor/miniz.c`**: A single-file implementation of Zlib/ZIP, used to create the CBZ output.
- **`src/version.hpp.in`**: Template for generating version information during construction.

### Dependencies

- **Poppler (cpp)**: Required for PDF parsing and rendering. Found via `pkg-config`.
- **miniz**: Bundled in `src/vendor/`.
- **C++17**: Required for standard filesystem support and concurrency features.

## CI/CD and Release Process

The project uses GitHub Actions for automation, defined in [release.yml](file:///.github/workflows/release.yml).

1.  **Build Phase**: On every push with a tag matching `v*`, the CI builds the project for:
    - Linux (x64 and arm64)
    - macOS (x64 and arm64)
2.  **Release Phase**: Once builds are successful, a GitHub Release is automatically created, and the binaries (tar.gz) are uploaded.
3.  **Distribution Phase**: The CI automatically calculates SHA256 hashes and updates the [hcross/homebrew-tap](https://github.com/hcross/homebrew-tap) repository to make the new version available via `brew`.

## Development Guidelines

Follow these steps to contribute a new feature or fix a bug.

### 1. Problem Definition
Describe the feature or bug by creating a new issue on GitHub. Use the available tools (MCP or terminal) to ensure the issue is well-defined with clear requirements.

### 2. Branching
Create a dedicated branch for your work using the following naming convention:
- Features: `feat/name-of-the-feat`
- Bug fixes: `fix/name-of-problem`

### 3. Implementation
Develop the software to implement the feature or fix the issue. Ensure your code follows the existing style and architecture.

> [!NOTE]
> Testing approach is currently omitted from this guide and will be addressed in a future update.

### 4. Pull Request
Push your branch and create a Pull Request (PR) on GitHub. Your PR should include:
- **TLDR**: A 10-second summary of what this PR does.
- **Description**: A detailed explanation of the changes made.
- **Flaws/Awareness**: Highlight any known limitations, specific complexities, or portions requiring extra attention during review.

### 5. Review & Merge
After the PR is reviewed and validated by a maintainer, it can be merged into the `main` branch.

### 6. Release & Versioning
To release a new version:
1.  Ensure `main` is stable.
2.  Create a new tag (e.g., `git tag v1.x.x`).
3.  Push the tag to GitHub (`git push --tags`).
This action triggers the CI/CD pipeline for build, release, and distribution.
