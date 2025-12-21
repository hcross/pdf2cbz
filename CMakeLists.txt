cmake_minimum_required(VERSION 3.17)
project(pdf2cbz VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find PkgConfig
find_package(PkgConfig REQUIRED)

# Check for Poppler
pkg_check_modules(POPPLER REQUIRED poppler-cpp)

message(STATUS "Poppler include: ${POPPLER_INCLUDE_DIRS}")
message(STATUS "Poppler libs: ${POPPLER_LIBRARIES}")
message(STATUS "Poppler lib dirs: ${POPPLER_LIBRARY_DIRS}")

include_directories(${POPPLER_INCLUDE_DIRS} src/vendor)
link_directories(${POPPLER_LIBRARY_DIRS})

add_executable(pdf2cbz
    src/main.cpp
    src/converter.cpp
    src/vendor/miniz.c
)

target_link_libraries(pdf2cbz PRIVATE ${POPPLER_LIBRARIES})
