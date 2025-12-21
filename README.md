# pdf2cbz

A lightweight C++ CLI tool to convert PDF files into CBZ (Comic Book Archive) format.

It extracts pages from a PDF document, renders them as images, and archives them into a zip file with the `.cbz` extension, making it compatible with most comic book readers.

## Features

- **Fast & Efficient**: Built with C++17.
- **High Quality**: Renders pages at 150 DPI (good balance for reading).
- **Cross-Platform**: Compatible with macOS, Linux, and Windows (verified on macOS).
- **Simple**: Single-purpose CLI utility.

## Prerequisites

Before building, ensure you have the following installed:

- **C++ Compiler** (supporting C++17)
- **CMake** (3.17 or later)
- **Poppler** (with C++ bindings)
- **pkg-config**

### Installation of Dependencies

**macOS**
```bash
brew install poppler pkg-config
```

**Linux (Debian/Ubuntu)**
```bash
sudo apt-get install libpoppler-cpp-dev pkg-config build-essential cmake
```

**Windows**
Recommended to use [vcpkg](https://github.com/microsoft/vcpkg):
```powershell
vcpkg install poppler
```

## Build Instructions

1.  Clone the repository:
    ```bash
    git clone <repository_url>
    cd pdf2cbz
    ```

2.  Create a build directory and compile:
    ```bash
    mkdir build
    cd build
    cmake ..
    make
    ```

3.  The executable `pdf2cbz` will be created in the `build` directory.

## Usage

```bash
./pdf2cbz <input.pdf> [output.cbz] [--parallel|-p]
```

- **input.pdf**: Path to the PDF file you want to convert.
- **output.cbz**: (Optional) Path for the output CBZ file. If omitted, the tool will use the input filename with a `.cbz` extension.
- **--parallel** / **-p**: Enable parallel processing (uses up to 5 threads) for faster conversion.

### Example

```bash
# Basic usage
./pdf2cbz comics.pdf

# Parallel mode (faster)
./pdf2cbz comics.pdf -p
```

## License

This project is licensed under the **GNU General Public License v3.0 (GPLv3)**.
See the [LICENSE](LICENSE) file for more details.

### Third-Party Licenses
- [miniz](https://github.com/richgel999/miniz) (MIT-like/Unlicense)
- [Poppler](https://poppler.freedesktop.org/) (GPLv2/v3)
