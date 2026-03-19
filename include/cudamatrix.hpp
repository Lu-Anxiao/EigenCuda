#ifndef EIGENCUDA_H_
#define EIGENCUDA_H_

#include <Eigen/Core>
#include <Eigen/Dense>
#ifdef FORCE_MUSA
#include <musa_runtime.h>
#include <mublas.h>
#include <musa.h>
#else
#include <cublas_v2.h>
#include <curand.h>
#endif
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

/*
 * \brief Perform Tensor-matrix multiplications in a GPU
 *
 * The `CudaPipeline` class handles the allocation and deallocation of arrays on
 * the GPU.
 */

namespace eigencuda {

#ifdef FORCE_MUSA
musaError_t checkMusa(musaError_t result);
#else
cudaError_t checkCuda(cudaError_t result);
#endif

using Index = Eigen::Index;
Index count_available_gpus();

class CudaMatrix {
 public:
  Index size() const { return _rows * _cols; };
  Index rows() const { return _rows; };
  Index cols() const { return _cols; };
  double *data() const { return _data.get(); };

  CudaMatrix(const Eigen::MatrixXd &matrix, 
#ifdef FORCE_MUSA
             const musaStream_t &stream
#else
             const cudaStream_t &stream
#endif
             );

  // Allocate memory in the GPU for a matrix
  CudaMatrix(Index nrows, Index ncols, 
#ifdef FORCE_MUSA
             const musaStream_t &stream
#else
             const cudaStream_t &stream
#endif
             );

  // Convert A Cudamatrix to an EigenMatrix
  operator Eigen::MatrixXd() const;

  void copy_to_gpu(const Eigen::MatrixXd &A);

 private:
  // Unique pointer with custom delete function
  using Unique_ptr_to_GPU_data = std::unique_ptr<double, void (*)(double *)>;

  Unique_ptr_to_GPU_data alloc_matrix_in_gpu(size_t size_arr) const;

  void throw_if_not_enough_memory_in_gpu(size_t requested_memory) const;

  size_t size_matrix() const { return this->size() * sizeof(double); }

  // Attributes of the matrix in the device
  Unique_ptr_to_GPU_data _data{nullptr,
#ifdef FORCE_MUSA
                               [](double *x) { checkMusa(musaFree(x)); }
#else
                               [](double *x) { checkCuda(cudaFree(x)); }
#endif
                               };
#ifdef FORCE_MUSA
  musaStream_t _stream = nullptr;
#else
  cudaStream_t _stream = nullptr;
#endif
  Index _rows;
  Index _cols;
};

}  // namespace eigencuda

#endif  // EIGENCUDA_H_
