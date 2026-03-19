
add_library(eigencuda cudamatrix.cc cudapipeline.cc)

target_include_directories(eigencuda
  PUBLIC
  ${PROJECT_SOURCE_DIR}/include
)

if(USE_MUSA)
  if(MUSA_INCLUDE_DIR)
    target_include_directories(eigencuda
      PUBLIC
      ${MUSA_INCLUDE_DIR}
    )
  endif()
  # Define the FORCE_MUSA macro
  target_compile_definitions(eigencuda PUBLIC FORCE_MUSA)
else()
  target_include_directories(eigencuda
    PUBLIC
    ${CUDA_INCLUDE_DIRS}
  )
endif()

set_target_properties(eigencuda
  PROPERTIES
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON
  )

target_compile_options(eigencuda
  PUBLIC
  "-Wall"
)

target_link_libraries(eigencuda
  PUBLIC
    Eigen3::Eigen
)

if(USE_MUSA)
  if(MUSA_LIBRARY)
    target_link_libraries(eigencuda
      PUBLIC
      ${MUSA_LIBRARY}
    )
  endif()
  # Try to find and link mublas if available
  find_library(MUSBLAS_LIBRARY
    NAMES mublas
    PATHS /usr/local/musa/lib64
          /opt/musa/lib64
          $ENV{MUSA_PATH}/lib64
          $ENV{MUSA_ROOT}/lib64
    NO_DEFAULT_PATH
  )
  if(MUSBLAS_LIBRARY)
    target_link_libraries(eigencuda
      PUBLIC
      ${MUSBLAS_LIBRARY}
    )
  endif()
  
  # Also link pthread which is commonly needed with MUSA
  find_package(Threads REQUIRED)
  target_link_libraries(eigencuda
    PUBLIC
    Threads::Threads
  )
else()
  target_link_libraries(eigencuda
    PUBLIC
      ${CUDA_LIBRARIES}
      ${CUDA_CUBLAS_LIBRARIES}
  )
endif()

if(ENABLE_TESTING)
  add_subdirectory(tests)
endif()
