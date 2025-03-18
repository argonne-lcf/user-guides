# HIP on Aurora

ALCF experimentally supports applications which use the HIP programming model via [chipStar](https://github.com/CHIP-SPV/chipStar). 

## Example

Below is a simple example of setting the HIP environment, building, and running a HIP code on Aurora. 

### Setting the environment to use HIP on Aurora

```bash linenums="1"
module use /soft/modulefiles
module load chipStar/1.2.1
```

With this, the `hipcc` executable should be in your path. For projects using CMake, there is support via `find_package(hip CONFIG REQUIRED)` which can be used with chipStar.

```cpp linenums="1" title="saxpy.cpp"
#include <stdio.h>
#include <math.h>
#include "hip/hip_runtime.h"

#define HIP_ASSERT(x) (assert((x)==hipSuccess))

__global__
void saxpy(int n, float a, float *x, float *y)
{
  int i = hipBlockDim_x * hipBlockIdx_x + hipThreadIdx_x;
  if (i < n) y[i] = a*x[i] + y[i];
}

int main(void)
{
  int N = 1<<20;
  float *x, *y, *d_x, *d_y;
  x = (float*)malloc(N*sizeof(float));
  y = (float*)malloc(N*sizeof(float));

  HIP_ASSERT(hipMalloc(&d_x, N*sizeof(float)));
  HIP_ASSERT(hipMalloc(&d_y, N*sizeof(float)));

  for (int i = 0; i < N; i++) {
    x[i] = 1.0f;
    y[i] = 2.0f;
  }

  HIP_ASSERT(hipMemcpy(d_x, x, N*sizeof(float), hipMemcpyHostToDevice));
  HIP_ASSERT(hipMemcpy(d_y, y, N*sizeof(float), hipMemcpyHostToDevice));

  // Perform SAXPY on 1M elements
  hipLaunchKernelGGL(saxpy,(N+255)/256, 256,0,0,N, 2.0f, d_x, d_y );

  HIP_ASSERT(hipMemcpy(y, d_y, N*sizeof(float), hipMemcpyDeviceToHost));

  float maxError = 0.0f;
  for (int i = 0; i < N; i++)
    maxError = ( maxError > abs(y[i]-4.0f) ) ? maxError : abs(y[i]-4.0f) ;
  printf("Max error: %f\n", maxError);

  HIP_ASSERT(hipFree(d_x));
  HIP_ASSERT(hipFree(d_y));
  free(x);
  free(y);
}
```

### Building HIP code on Aurora

```console
> hipcc saxpy.cpp
```

### Running HIP code on Aurora

```console
> ./a.out
Max error: 0.000000
```

There are additional details in the [chipStar user documentation](https://github.com/CHIP-SPV/chipStar/blob/main/docs/Using.md).
