cmake_minimum_required(VERSION 3.17)
project(pdf2cbz VERSION 1.0)

# Allow version override from command line (used in CI)
if(VERSION_OVERRIDE)
    set(PROJECT_VERSION ${VERSION_OVERRIDE})
endif()

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/version.hpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/version.hpp"
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_TESTS "Enable unit tests" OFF)

if(ENABLE_TESTS)
    cmake_policy(SET CMP0135 NEW)
    include(FetchContent)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

# Find PkgConfig
find_package(PkgConfig REQUIRED)

# Check for Poppler
pkg_check_modules(POPPLER REQUIRED IMPORTED_TARGET poppler-cpp)

message(STATUS "Poppler include: ${POPPLER_INCLUDE_DIRS}")
message(STATUS "Poppler libs: ${POPPLER_LIBRARIES}")
message(STATUS "Poppler lib dirs: ${POPPLER_LIBRARY_DIRS}")

# Create a library for the core logic
add_library(pdf2cbz_core STATIC
    src/converter.cpp
    src/vendor/miniz.c
)
target_include_directories(pdf2cbz_core PUBLIC src src/vendor "${CMAKE_CURRENT_BINARY_DIR}/generated")
target_link_libraries(pdf2cbz_core PUBLIC PkgConfig::POPPLER)

add_executable(pdf2cbz src/main.cpp)
target_link_libraries(pdf2cbz PRIVATE pdf2cbz_core)

if(ENABLE_TESTS)
    enable_testing()

    add_executable(pdf2cbz_tests
        tests/utils_test.cpp
        tests/converter_test.cpp
    )
    target_link_libraries(pdf2cbz_tests PRIVATE pdf2cbz_core gtest_main)

    add_test(NAME pdf2cbz_tests COMMAND pdf2cbz_tests)
endif()
