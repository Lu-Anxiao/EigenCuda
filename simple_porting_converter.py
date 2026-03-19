#!/usr/bin/env python3
"""
CUDA to MUSA converter for EigenCuda repository
"""

import os
import re
import shutil
from pathlib import Path


# Define CUDA to MUSA mapping
CUDA_TO_MUSA_MAP = {
    # Headers
    r'#include [<"]cuda_runtime\.h[>"]': '#include <musa_runtime.h>',
    r'#include [<"]cublas_v2\.h[>"]': '#include <musblas.h>',
    r'#include [<"]curand\.h[>"]': '#include <musa.h>',
    
    # Error types
    r'\bcudaError_t\b': 'musaError_t',
    r'\bcudaSuccess\b': 'musaSuccess',
    r'\bcudaGetErrorString\b': 'musaGetErrorString',
    
    # Stream types and functions
    r'\bcudaStream_t\b': 'musaStream_t',
    r'\bcudaStreamCreate\b': 'musaStreamCreate',
    r'\bcudaStreamSynchronize\b': 'musaStreamSynchronize',
    r'\bcudaStreamDestroy\b': 'musaStreamDestroy',
    
    # Memory functions
    r'\bcudaMalloc\b': 'musaMalloc',
    r'\bcudaFree\b': 'musaFree',
    r'\bcudaMemcpyAsync\b': 'musaMemcpyAsync',
    r'\bcudaMemcpyHostToDevice\b': 'musaMemcpyHostToDevice',
    r'\bcudaMemcpyDeviceToHost\b': 'musaMemcpyDeviceToHost',
    r'\bcudaMemGetInfo\b': 'musaMemGetInfo',
    
    # Device functions
    r'\bcudaGetDeviceCount\b': 'musaGetDeviceCount',
    
    # CUBLAS types and functions
    r'\bcublasHandle_t\b': 'musblasHandle_t',
    r'\bcublasCreate\b': 'musblasCreate',
    r'\bcublasDestroy\b': 'musblasDestroy',
    r'\bcublasDgemm\b': 'musblasDgemm',
    r'\bCUBLAS_OP_N\b': 'MUSBLAS_OP_N',
    
    # General CUDA prefix
    r'\bcuda\b': 'musa',
}


def replace_patterns(text, mapping_dict):
    """Apply all pattern replacements in the mapping dictionary."""
    for pattern, replacement in mapping_dict.items():
        # Compile the regex pattern
        compiled_pattern = re.compile(pattern)
        # Apply the replacement
        text = compiled_pattern.sub(replacement, text)
    return text


def process_file(filepath, output_dir):
    """Process a single file and create a MUSA version."""
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # Apply all transformations
    transformed_content = replace_patterns(content, CUDA_TO_MUSA_MAP)
    
    # Write the transformed content to the output directory
    output_path = output_dir / filepath.name
    with open(output_path, 'w', encoding='utf-8') as f:
        f.write(transformed_content)
    
    print(f"Processed: {filepath} -> {output_path}")


def main():
    # Define paths
    repo_root = Path('.')
    src_dir = repo_root / 'src'
    include_dir = repo_root / 'include'
    
    # Create *_musa directories
    src_musa_dir = repo_root / 'src/musa'
    include_musa_dir = repo_root / 'include/musa'
    
    src_musa_dir.mkdir(parents=True, exist_ok=True)
    include_musa_dir.mkdir(parents=True, exist_ok=True)
    
    print("Created musa directories.")
    
    # Process all .cc and .cu files in src
    for src_file in src_dir.glob('*.[ch][cu]*'):
        process_file(src_file, src_musa_dir)
    
    # Process all .h and .hpp files in include
    for inc_file in include_dir.glob('*.[hH][pP]*'):
        process_file(inc_file, include_musa_dir)
    
    # Also process test files
    test_dir = src_dir / 'tests'
    if test_dir.exists():
        test_musa_dir = src_musa_dir / 'tests'
        test_musa_dir.mkdir(exist_ok=True)
        
        for test_file in test_dir.glob('*.cc'):
            process_file(test_file, test_musa_dir)
    
    print("Conversion completed!")


if __name__ == "__main__":
    main()