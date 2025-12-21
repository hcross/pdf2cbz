# pdf2cbz

A fast C++ CLI tool to convert PDF files into CBZ (Comic Book Archive) format with parallel processing support.

## Installation

### Via Homebrew (macOS & Linux)

The easiest way to install `pdf2cbz` is via the official tap:

```bash
brew tap hcross/tap
brew install pdf2cbz
```

### Build from Source

If you prefer to compile it yourself, ensure you have a C++17 compiler, CMake, and the Poppler development library installed.

#### 1. Install Dependencies

**macOS**
```bash
brew install poppler pkg-config cmake
```

**Linux (Ubuntu/Debian)**
```bash
sudo apt-get install libpoppler-cpp-dev pkg-config build-essential cmake
```

#### 2. Compile

```bash
git clone https://github.com/hcross/pdf2cbz.git
cd pdf2cbz
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

The executable `pdf2cbz` will be located in the `build/` directory.

## Usage

```bash
./pdf2cbz <input.pdf> [output.cbz] [--threads|-t <N>]
```

- **input.pdf**: Path to the source PDF.
- **output.cbz**: (Optional) Destination path. Defaults to input name with `.cbz`.
- **--threads / -t**: (Optional) Number of parallel threads. Defaults to your CPU core count.

### Example

```bash
# Convert using all available cores
./pdf2cbz book.pdf

# Convert using exactly 4 threads
./pdf2cbz book.pdf -t 4
```

## License

This project is licensed under the **GNU General Public License v3.0 (GPLv3)**.
See the [LICENSE](LICENSE) file for more details.
