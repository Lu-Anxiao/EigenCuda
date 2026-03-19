#!/bin/bash

# Script to apply SimplePorting CUDA to MUSA transformations to the EigenCuda project

echo "Starting CUDA to MUSA migration for EigenCuda..."

# Create *_musa directories as per SimplePorting convention
mkdir -p src/musa
mkdir -p include/musa

# Copy original files to musa directories
cp src/*.cc src/musa/
cp include/*.hpp include/musa/

# Apply systematic transformations to the copied files
echo "Applying API transformations..."

# Transform all the copied files
for file in src/musa/*.cc include/musa/*.hpp; do
  echo "Processing $file..."
  
  # General CUDA to MUSA transformations
  sed -i 's/\bcudaError_t\b/musaError_t/g' "$file"
  sed -i 's/\bcudaStream_t\b/musaStream_t/g' "$file"
  sed -i 's/\bcublasHandle_t\b/musblasHandle_t/g'
  sed -i 's/\bCUBLAS_OP_N\b/MUSBLAS_OP_N/g' "$file"
  sed -i 's/\bcudaSuccess\b/musaSuccess/g' "$file"
  sed -i 's/\bcudaGetErrorString\b/musaGetErrorString/g' "$file"
  sed -i 's/\bcudaGetDeviceCount\b/musaGetDeviceCount/g' "$file"
  sed -i 's/\bcudaMemcpyAsync\b/musaMemcpyAsync/g' "$file"
  sed -i 's/\bcudaMemcpyHostToDevice\b/musaMemcpyHostToDevice/g' "$file"
  sed -i 's/\bcudaMemcpyDeviceToHost\b/musaMemcpyDeviceToHost/g' "$file"
  sed -i 's/\bcudaStreamSynchronize\b/musaStreamSynchronize/g' "$file"
  sed -i 's/\bcudaStreamCreate\b/musaStreamCreate/g' "$file"
  sed -i 's/\bcudaStreamDestroy\b/musaStreamDestroy/g' "$file"
  sed -i 's/\bcudaMalloc\b/musaMalloc/g' "$file"
  sed -i 's/\bcudaFree\b/musaFree/g' "$file"
  sed -i 's/\bcudaMemGetInfo\b/musaMemGetInfo/g' "$file"
  sed -i 's/\bcublasCreate\b/musblasCreate/g' "$file"
  sed -i 's/\bcublasDestroy\b/musblasDestroy/g' "$file"
  sed -i 's/\bcublasDgemm\b/musblasDgemm/g' "$file"
  
  # Update header includes
  sed -i 's/\bcublas_v2.h\b/musblas.h/g' "$file"
  sed -i 's/\bcuda_runtime.h\b/musa_runtime.h/g' "$file"
  sed -i 's/\bcuda.h\b/musa.h/g' "$file"
done

echo "Migration completed. Check the src/musa/ and include/musa/ directories."

echo "Creating symlinks for migrated files..."
# Create symlinks from original locations to migrated files for building
ln -sf ../src/musa/cudamatrix.cc src/cudamatrix_musa.cc
ln -sf ../src/musa/cudapipeline.cc src/cudapipeline_musa.cc