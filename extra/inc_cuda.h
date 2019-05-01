#pragma once
/*---------------------------------------------------- cuda api*/
#define CUDA_NO_HALF

#include <cuda.h>
#include <cuda_runtime.h>
#include <cuda_runtime_api.h>
#include <cuda_gl_interop.h>

/*---------------------------------------------------- thrust sdk*/
#include <thrust/execution_policy.h>
#include <thrust/system/cpp/execution_policy.h>
#include <thrust/system/omp/execution_policy.h>
#include <thrust/system/tbb/execution_policy.h>
#include <thrust/system/cuda/execution_policy.h>

#include <thrust/host_vector.h>
#include <thrust/device_vector.h>

#include <thrust/sort.h>
#include <thrust/sequence.h>


#include <thrust/remove.h>
#include <thrust/random.h>
#include <thrust/for_each.h>

#include <thrust/iterator/zip_iterator.h>

