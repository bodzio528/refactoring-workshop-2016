cmake_minimum_required(VERSION 3.0)
project(RefactoringWorkshop LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -pedantic -fno-rtti)

# coverage (GCC)
option(BUILD_COVERAGE_UNIT_TESTS "Decide whether generate coverage report for unit tests" OFF)
if (BUILD_COVERAGE_UNIT_TESTS)
    set(COVERAGE_REPORT_LOCATION "CoverageReport" CACHE STRING "Location for HTML coverage report." FORCE)

    set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)
    include(CodeCoverage)
endif()

# common libs
add_subdirectory(googletest-master)
add_subdirectory(DynamicEvents)

add_subdirectory(SnakeController)
