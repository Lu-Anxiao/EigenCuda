# Migration Progress: EigenCuda

## Project Overview
- Repository: https://github.com/NLESC-JCER/EigenCuda
- Purpose: Offload Eigen3 matrix operations to GPU using CUDA/CUBLAS, now migrating to MUSA
- Type: C++ Library with CUDA kernels
- Number of CUDA files: 0 .cu files, but several .cc files with CUDA calls

## CUDA Components Identified
1. CUDA Runtime API calls:
   - cudaMalloc, cudaFree
   - cudaMemcpyAsync
   - cudaStreamCreate, cudaStreamSynchronize, cudaStreamDestroy
   - cudaGetDeviceCount
   - cudaMemGetInfo
   - cudaError_t, cudaStream_t

2. CUDA Driver/CUBLAS API calls:
   - cublasCreate, cublasDestroy
   - cublasDgemm
   - cublasHandle_t, cublasOperation_t

3. Header files:
   - cudamatrix.hpp
   - cudapipeline.hpp

4. Source files:
   - cudamatrix.cc
   - cudapipeline.cc

5. Test files:
   - test_dot.cc

## Migration Approach
Implemented conditional compilation approach to support both CUDA and MUSA targets by defining the `FORCE_MUSA` macro when building with MUSA.

### Changes Made

#### Header Files
- `include/cudamatrix.hpp`: Added conditional includes for CUDA/MUSA headers using `#ifdef FORCE_MUSA`
- `include/cudapipeline.hpp`: Added conditional includes and implementations for CUDA/MUSA APIs

#### Source Files
- `src/cudamatrix.cc`: Implemented conditional compilation for all CUDA/MUSA API calls
- `src/cudapipeline.cc`: Implemented conditional compilation for all CUDA/MUSA API calls

#### Build System
- `CMakeLists.txt`: Added `USE_MUSA` and `FORCE_MUSA` options to control which backend to use
- Updated linking to use appropriate CUDA or MUSA libraries
- Updated include directories to support both backends

#### API Mappings Applied
- cudaError_t → musaError_t (when FORCE_MUSA defined)
- cudaSuccess → musaSuccess
- cudaGetErrorString → musaGetErrorString
- cudaGetDeviceCount → musaGetDeviceCount
- cudaMalloc → musaMalloc
- cudaFree → musaFree
- cudaMemcpyAsync → musaMemcpyAsync
- cudaMemcpyHostToDevice → musaMemcpyHostToDevice
- cudaMemcpyDeviceToHost → musaMemcpyDeviceToHost
- cudaStreamCreate → musaStreamCreate
- cudaStreamSynchronize → musaStreamSynchronize
- cudaStreamDestroy → musaStreamDestroy
- cudaMemGetInfo → musaMemGetInfo
- cublasHandle_t → mublasHandle_t
- cublasCreate → mublasCreate
- cublasDestroy → mublasDestroy
- cublasDgemm → mublasDgemm
- CUBLAS_OP_N → MUBLAS_OP_N

## Build Instructions
To build with CUDA (default):
```
cmake -H. -Bbuild && cmake --build build
```

To build with MUSA:
```
cmake -H. -Bbuild -DUSE_MUSA=ON -DFORCE_MUSA=ON && cmake --build build
```

## Issues Encountered
- Had to implement conditional compilation approach to maintain compatibility while enabling MUSA
- Required careful handling of type definitions that differ between CUDA and MUSA APIs

## Status
- Code converted: Complete
- Build system updated: Complete
- Ready for MUSA compilation: Yes
- Verification needed: Pending

## Next Steps
1. Verify successful compilation with MUSA
2. Run tests with MUSA backend
3. Verify functionality equivalence