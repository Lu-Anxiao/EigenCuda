#ifndef CUDA_PIPELINE__H
#define CUDA_PIPELINE__H

#include "cudamatrix.hpp"
#ifdef FORCE_MUSA
#include <mublas.h>
#endif

/*
 * \brief Perform Tensor-matrix multiplications in a GPU
 *
 * The `CudaPipeline` class handles the allocation and deallocation of arrays on
 * the GPU.
 */

namespace eigencuda {

/* \brief The CudaPipeline class offload Eigen operations to an *Nvidia* GPU
 * using the CUDA language. The Cublas handle is the context manager for all the
 * resources needed by Cublas. While a stream is a queue of sequential
 * operations executed in the Nvidia device.
 */
class CudaPipeline {
 public:
  CudaPipeline() {
#ifdef FORCE_MUSA
    mublasCreate(&_handle);
    musaStreamCreate(&_stream);
#else
    cublasCreate(&_handle);
    cudaStreamCreate(&_stream);
#endif
  }
  ~CudaPipeline();

  CudaPipeline(const CudaPipeline &) = delete;
  CudaPipeline &operator=(const CudaPipeline &) = delete;

  // Invoke the ?gemm function of cublas
  void gemm(const CudaMatrix &A, const CudaMatrix &B, CudaMatrix &C) const;

  const 
#ifdef FORCE_MUSA
  musaStream_t
#else
  cudaStream_t
#endif
  &get_stream() const { return _stream; };

 private:
  // The BLAS handles allocates hardware resources on the host and device.
#ifdef FORCE_MUSA
  mublasHandle_t _handle;
  musaStream_t _stream;
#else
  cublasHandle_t _handle;
  cudaStream_t _stream;
#endif
};

}  // namespace eigencuda

#endif
