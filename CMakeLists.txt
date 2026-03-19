# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.5)

# Project's name
project(EigenCuda LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CXX_FLAGS)
  #release comes with -O3 by default
  set(CMAKE_BUILD_TYPE Release CACHE STRING
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CXX_FLAGS)

# User input options
option(ENABLE_TESTING "Build and enable tests" OFF)
if(ENABLE_TESTING)
  enable_testing()
  find_package(Boost REQUIRED COMPONENTS unit_test_framework)
endif(ENABLE_TESTING)

# Check if MUSA is enabled
option(USE_MUSA "Use MUSA instead of CUDA" OFF)
option(FORCE_MUSA "Force MUSA compilation in code" OFF)

if(USE_MUSA)
  # For MUSA, we use environment variables or standard paths
  # Users need to ensure MUSA SDK is properly installed and PATH/LD_LIBRARY_PATH are set
  message(STATUS "Using MUSA backend (please ensure MUSA SDK is installed)")
  set(MUSA_FOUND TRUE)
  # Common MUSA installation paths
  find_path(MUSA_INCLUDE_DIR musa_runtime.h
    PATHS /usr/local/musa/include
          /opt/musa/include
          $ENV{MUSA_PATH}/include
          $ENV{MUSA_ROOT}/include
    NO_DEFAULT_PATH
  )
  find_library(MUSA_LIBRARY
    NAMES musart
    PATHS /usr/local/musa/lib64
          /opt/musa/lib64
          $ENV{MUSA_PATH}/lib64
          $ENV{MUSA_ROOT}/lib64
    NO_DEFAULT_PATH
  )
else()
  # Search for Cuda
  find_package(CUDA REQUIRED)
endif()

# search for Eigen
find_package (Eigen3 3.3 REQUIRED CONFIG
  HINTS ${CONDA_PREFIX}/include
)

message(STATUS "EIGEN Include: " ${EIGEN3_INCLUDE_DIR})

add_subdirectory(src)
