#!/bin/bash
# Check script to verify CUDA to MUSA conversion completeness

echo "Checking CUDA to MUSA conversion completeness..."

# Change to the repository root
cd "$(dirname "$0")"

echo ""
echo "Running grep checks to verify conversion completeness..."

echo ""
echo "1. Checking for remaining CUDA header includes:"
grep -r "#include.*cuda" include/ src/ || echo "No CUDA headers found - OK"

echo ""
echo "2. Checking for remaining CUDA function calls:"
grep -r "cuda[A-Z]" include/ src/ || echo "No CUDA function calls found - OK"

echo ""
echo "3. Checking for remaining cuBLAS function calls:"
grep -r "cublas" include/ src/ || echo "No cuBLAS calls found - OK"

echo ""
echo "4. Checking for remaining CUDA type definitions:"
grep -r "cudaError_t\|cudaStream_t\|cublasHandle_t\|CUBLAS_OP" include/ src/ || echo "No CUDA types found - OK"

echo ""
echo "5. Checking for remaining CUDA memory functions:"
grep -r "cudaMalloc\|cudaFree\|cudaMemcpy\|cudaStreamS\|cudaMemGetInfo\|cudaGetDevice" include/ src/ || echo "No CUDA memory functions found - OK"

echo ""
echo "6. Checking for conditional compilation markers:"
grep -r "FORCE_MUSA" include/ src/ && echo "Found conditional compilation - OK" || echo "Conditional compilation not found!"

echo ""
echo "Conversion completeness check completed."