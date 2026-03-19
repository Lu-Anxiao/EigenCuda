# Building EigenCuda with MUSA Support

## Overview
This guide explains how to build the EigenCuda library with MUSA (Moore Threads Universal System Architecture) support instead of CUDA.

## Prerequisites
- MUSA SDK installed on your system (version 4.x or higher)
- Moore Threads GPU drivers properly configured
- Ensure the MUSA libraries are in your library path

## Setting Up MUSA Environment

Before building with MUSA support, you need to make sure the MUSA SDK is properly installed and accessible. Typically:

```bash
export MUSA_PATH=/usr/local/musa  # or wherever your MUSA SDK is installed
export LD_LIBRARY_PATH=$MUSA_PATH/lib64:$LD_LIBRARY_PATH
export PATH=$MUSA_PATH/bin:$PATH
```

## Normal CUDA Build (Default)
```bash
# Standard build using CUDA
cmake -H. -Bbuild && cmake --build build
```

## MUSA Build
```bash
# Build using MUSA (will compile with MUSA API calls instead of CUDA)
cmake -H. -Bbuild_musa -DUSE_MUSA=ON -DFORCE_MUSA=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build_musa
```

## Using the Library

The library maintains the same API regardless of whether it's built with CUDA or MUSA backend. The `FORCE_MUSA` flag controls which backend is used at compile-time.

## Conditional Compilation Details

The code uses the following conditional compilation:
- When `FORCE_MUSA` is defined: Uses MUSA API calls
- When `FORCE_MUSA` is not defined: Uses CUDA API calls (default)

API mappings implemented:
- CUDA Runtime API → MUSA Runtime API
- cuBLAS → MUSBLAS
- All CUDA types → MUSA equivalents

## Testing

Tests can be enabled with:
```bash
cmake -H. -Bbuild -DENABLE_TESTING=ON
cmake --build build
ctest --test-dir build
```

This will work with both CUDA and MUSA builds.