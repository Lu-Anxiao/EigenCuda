#include "cudamatrix.hpp"
#ifdef FORCE_MUSA
#include <musa_runtime.h>
#include <mublas.h>
#else
#include <cuda_runtime.h>
#include <cublas_v2.h>
#endif

namespace eigencuda {

#ifdef FORCE_MUSA
musaError_t checkMusa(musaError_t result) {
#if defined(DEBUG)
  if (result != musaSuccess) {
    std::cerr << "MUSA Runtime Error: " << musaGetErrorString(result) << "\n";
  }
#endif
  return result;
}
#else
cudaError_t checkCuda(cudaError_t result) {
#if defined(DEBUG)
  if (result != cudaSuccess) {
    std::cerr << "CUDA Runtime Error: " << cudaGetErrorString(result) << "\n";
  }
#endif
  return result;
}
#endif

Index count_available_gpus() {
  int count;
#ifdef FORCE_MUSA
  musaError_t err = musaGetDeviceCount(&count);
  return 0 ? (err != musaSuccess) : Index(count);
#else
  cudaError_t err = cudaGetDeviceCount(&count);
  return 0 ? (err != cudaSuccess) : Index(count);
#endif
}

CudaMatrix::CudaMatrix(const Eigen::MatrixXd &matrix,
#ifdef FORCE_MUSA
                       const musaStream_t &stream
#else
                       const cudaStream_t &stream
#endif
                       )
    : _rows{static_cast<Index>(matrix.rows())},
      _cols{static_cast<Index>(matrix.cols())} {
  _data = alloc_matrix_in_gpu(size_matrix());
  _stream = stream;
#ifdef FORCE_MUSA
  musaError_t err = musaMemcpyAsync(_data.get(), matrix.data(), size_matrix(),
                                    musaMemcpyHostToDevice, stream);
#else
  cudaError_t err = cudaMemcpyAsync(_data.get(), matrix.data(), size_matrix(),
                                    cudaMemcpyHostToDevice, stream);
#endif
  if (err != 0) {
    throw std::runtime_error("Error copy arrays to device");
  }
}

CudaMatrix::CudaMatrix(Index nrows, Index ncols, 
#ifdef FORCE_MUSA
                       const musaStream_t &stream
#else
                       const cudaStream_t &stream
#endif
                       )
    : _rows{static_cast<Index>(nrows)}, _cols{static_cast<Index>(ncols)} {
  _data = alloc_matrix_in_gpu(size_matrix());
  _stream = stream;
}

CudaMatrix::operator Eigen::MatrixXd() const {
  Eigen::MatrixXd result = Eigen::MatrixXd::Zero(this->rows(), this->cols());
#ifdef FORCE_MUSA
  checkMusa(musaMemcpyAsync(result.data(), this->data(), this->size_matrix(),
                            musaMemcpyDeviceToHost, this->_stream));
  checkMusa(musaStreamSynchronize(this->_stream));
#else
  checkCuda(cudaMemcpyAsync(result.data(), this->data(), this->size_matrix(),
                            cudaMemcpyDeviceToHost, this->_stream));
  checkCuda(cudaStreamSynchronize(this->_stream));
#endif
  return result;
}

void CudaMatrix::copy_to_gpu(const Eigen::MatrixXd &A) {
  size_t size_A = static_cast<Index>(A.size()) * sizeof(double);
#ifdef FORCE_MUSA
  checkMusa(musaMemcpyAsync(this->data(), A.data(), size_A,
                            musaMemcpyHostToDevice, _stream));
#else
  checkCuda(cudaMemcpyAsync(this->data(), A.data(), size_A,
                            cudaMemcpyHostToDevice, _stream));
#endif
}

CudaMatrix::Unique_ptr_to_GPU_data CudaMatrix::alloc_matrix_in_gpu(
    size_t size_arr) const {
  double *dmatrix;
  throw_if_not_enough_memory_in_gpu(size_arr);
#ifdef FORCE_MUSA
  checkMusa(musaMalloc(&dmatrix, size_arr));
  Unique_ptr_to_GPU_data dev_ptr(dmatrix,
                                 [](double *x) { checkMusa(musaFree(x)); });
#else
  checkCuda(cudaMalloc(&dmatrix, size_arr));
  Unique_ptr_to_GPU_data dev_ptr(dmatrix,
                                 [](double *x) { checkCuda(cudaFree(x)); });
#endif
  return dev_ptr;
}

void CudaMatrix::throw_if_not_enough_memory_in_gpu(
    size_t requested_memory) const {
  size_t free, total;
#ifdef FORCE_MUSA
  checkMusa(musaMemGetInfo(&free, &total));
#else
  checkCuda(cudaMemGetInfo(&free, &total));
#endif

  std::ostringstream oss;
  oss << "There were requested : " << requested_memory
      << "bytes Index the device\n";
  oss << "Device Free memory (bytes): " << free
      << "\nDevice total Memory (bytes): " << total << "\n";

  // Raise an error if there is not enough total or free memory in the device
  if (requested_memory > free) {
    oss << "There is not enough memory in the Device!\n";
    throw std::runtime_error(oss.str());
  }
}

}  // namespace eigencuda
