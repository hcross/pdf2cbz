# Distribution Guide

This document provides templates and instructions for making `pdf2cbz` available on package managers like Homebrew (macOS/Linux) and Chocolatey (Windows).

## Homebrew (macOS & Linux)

To provide a Homebrew formula, you can either submit it to `homebrew/core` (requires high usage) or create your own "Tap".

### Creating a Tap
1. Create a repository named `homebrew-tap` on GitHub.
2. Create a file named `pdf2cbz.rb` in that repository.

### Option 1: Source-Build Formula (Recommended for safety)
This template builds `pdf2cbz` on the user's machine. It works on both macOS and Linux.

```ruby
class Pdf2cbz < Formula
  desc "PDF to CBZ Converter with parallel processing support"
  homepage "https://github.com/hoanicross/pdf2cbz"
  url "https://github.com/hoanicross/pdf2cbz/archive/refs/tags/v1.0.0.tar.gz"
  sha256 "REPLACE_WITH_ACTUAL_SHA256"
  license "GPL-3.0-or-later"

  depends_on "cmake" => :build
  depends_on "pkg-config" => :build
  depends_on "poppler"

  def install
    system "cmake", "-S", ".", "-B", "build", *std_cmake_args
    system "cmake", "--build", "build"
    bin.install "build/pdf2cbz"
  end

  test do
    system "#{bin}/pdf2cbz", "--version"
  end
end
```

### Option 2: Binary Distribution Formula (Faster installation)
To avoid compilation on every install, you can distribute pre-built binaries. Update the `url` and `sha256` for each platform.

```ruby
class Pdf2cbz < Formula
  desc "PDF to CBZ Converter"
  homepage "https://github.com/hoanicross/pdf2cbz"
  version "1.0.0"

  on_macos do
    if Hardware::CPU.arm?
      url "https://github.com/hoanicross/pdf2cbz/releases/download/v1.0.0/pdf2cbz-macos-arm64.tar.gz"
      sha256 "..."
    else
      url "https://github.com/hoanicross/pdf2cbz/releases/download/v1.0.0/pdf2cbz-macos-x64.tar.gz"
      sha256 "..."
    end
  end

  on_linux do
    if Hardware::CPU.arm?
      url "https://github.com/hoanicross/pdf2cbz/releases/download/v1.0.0/pdf2cbz-linux-arm64.tar.gz"
      sha256 "..."
    else
      url "https://github.com/hoanicross/pdf2cbz/releases/download/v1.0.0/pdf2cbz-linux-x64.tar.gz"
      sha256 "..."
    end
  end

  def install
    # The binary might be named based on the artifact name in release.yml
    # Adjust the filename accordingly (e.g., pdf2cbz-linux-x64)
    # Or rename them during the packaging step in CI.
    bin.install "pdf2cbz"
  end

  test do
    system "#{bin}/pdf2cbz", "--version"
  end
end
```

Users can then install it with:
```bash
brew tap hoanicross/tap
brew install pdf2cbz
```

---

## Chocolatey (Windows)

Chocolatey requires a `.nuspec` file and PowerShell scripts.

### Package Template (`pdf2cbz.nuspec`)
```xml
<?xml version="1.0" encoding="utf-8"?>
<package xmlns="http://schemas.microsoft.com/packaging/2015/06/nuspec.xsd">
  <metadata>
    <id>pdf2cbz</id>
    <version>1.0.0</version>
    <title>pdf2cbz</title>
    <authors>Hoani Cross</authors>
    <projectUrl>https://github.com/hoanicross/pdf2cbz</projectUrl>
    <tags>pdf cbz converter comic</tags>
    <summary>PDF to CBZ Converter</summary>
    <description>A fast PDF to CBZ converter written in C++ with parallel processing.</description>
  </metadata>
  <files>
    <file src="tools\**" target="tools" />
  </files>
</package>
```

### Install Script (`tools\chocolateyInstall.ps1`)
You should bundle the Windows binary in the `tools` folder or download it.
```powershell
$ErrorActionPreference = 'Stop';
$packageName= 'pdf2cbz'
$toolsDir   = "$(Split-Path -parent $MyInvocation.MyCommand.Definition)"
$fileLocation = Join-Path $toolsDir 'pdf2cbz.exe'

Install-ChocolateyPath "$toolsDir"
```

*Note: For Windows, you'll need to update the GitHub Action to build using MinGW or MSVC via MSYS2 to handle the Poppler dependency.*
