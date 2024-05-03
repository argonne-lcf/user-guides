# LibTorch C++ Library

LibTorch is a C++ library for Torch, with many of the API that are available in PyTorch. Users can find more information on the [PyTorch documentation](https://pytorch.org/cppdocs/installing.html).
This is useful to integrate the Torch ML framework into traditional HPC simulation codes and therefore enable training and inferecing of ML models.
On Aurora, the Intel Extension for PyTorch (IPEX) library is needed to access the Max 1550 GPU, which have the device name `kXPU` in LibTorch. 
During compilation, Intel optimizations will be activated automatically once the IPEX dynamic library is linked.


## Environment Setup

To use LibTorch on Aurora, load the ML frameworks module
```
module use /soft/modulefiles
module load frameworks/2023.12.15.001
```
which will also load the consistent oneAPI SDK and `cmake`.


## Torch and IPEX libraries

With the ML frameworks module loaded as shown above, run
```
python -c 'import torch; print(torch.__path__[0])'
python -c 'import torch;print(torch.utils.cmake_prefix_path)'
```
to find the path to the Torch libraries, include files, and CMake files.

For the path to the IPEX dynamic library, run
```
python -c 'import torch; print(torch.__path__[0].replace("torch","intel_extension_for_pytorch"))'
```



## Model Inferencing Using the Torch API 
This example shows how to perform inference on the ResNet50 model using only the LibTorch API.
First, get a jit-traced version of the model running `resnet50_trace.py` below.
```
import torch
import torchvision
import intel_extension_for_pytorch as ipex
from time import perf_counter

device = 'xpu'

model = torchvision.models.resnet50()
model.to(device)
model.eval()

dummy_input = torch.rand(1, 3, 224, 224).to(device)

model_jit = torch.jit.trace(model, dummy_input)
tic = perf_counter()
predictions = model_jit(dummy_input)
toc = perf_counter()
print(f"Inference time: {toc-tic}")

torch.jit.save(model_jit, f"resnet50_jit.pt")
```

Then, use the source code in `inference-example.cpp`
```
#include <torch/torch.h>
#include <torch/script.h>
#include <iostream>

int main(int argc, const char* argv[]) {
    torch::jit::script::Module model;
    try {
        model = torch::jit::load(argv[1]);
        std::cout << "Loaded the model\n";
    }
    catch (const c10::Error& e) {
        std::cerr << "error loading the model\n";
        return -1;
    }
    // Upload model to GPU
    model.to(torch::Device(torch::kXPU));
    std::cout << "Model offloaded to GPU\n\n";

    auto options = torch::TensorOptions()
                      .dtype(torch::kFloat32)
                      .device(torch::kXPU);
    torch::Tensor input_tensor = torch::rand({1,3,224,224}, options);
    assert(input_tensor.dtype() == torch::kFloat32);
    assert(input_tensor.device().type() == torch::kXPU);
    std::cout << "Created the input tesor on GPU\n";

    torch::Tensor output = model.forward({input_tensor}).toTensor();
    std::cout << "Performed inference\n\n";

    std::cout << "Predicted tensor is : \n";
    std::cout << output.slice(/*dim=*/1, /*start=*/0, /*end=*/10) << '\n';

    return 0;
}
```

and the `CMakeLists.txt` file

```
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
cmake_policy(SET CMP0074 NEW)
project(inference-example)

find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS} -Wl,--no-as-needed")

add_executable(inference-example inference-example.cpp)
target_link_libraries(inference-example "${TORCH_LIBRARIES}" "${INTEL_EXTENSION_FOR_PYTORCH_PATH}/lib/libintel-ext-pt-gpu.so")

set_property(TARGET inference-example PROPERTY CXX_STANDARD 17)
```

to build the inference example.

Finally, execute the `doConfig.sh` script below
```
#!/bin/bash

cmake \
    -DCMAKE_PREFIX_PATH=`python -c 'import torch;print(torch.utils.cmake_prefix_path)'` \
    -DINTEL_EXTENSION_FOR_PYTORCH_PATH=`python -c 'import torch; print(torch.__path__[0].replace("torch","intel_extension_for_pytorch"))'` \
    ./

make
./inference-example ./resnet50_jit.pt
```

## LibTorch Interoperability with SYCL Pipelines
The LibTorch API can be integrated with data pilelines using SYCL to offload and operate on the input and output data on the Intel Max 1550 GPU. 
The code below extends the above example of performing inference with the ResNet50 model by first generating the input data on the CPU, then offloading it to the GPU with SYCL, and finally passing the device pointer to LibTorch for inference.

The source code for `inference-example.cpp` is modified as follows
```
#include <torch/torch.h>
#include <torch/script.h>
#include <iostream>
#include "sycl/sycl.hpp"
#include <vector>

const int N_BATCH = 1;
const int N_CHANNELS = 3;
const int N_PIXELS = 224;
const int INPUTS_SIZE = N_BATCH*N_CHANNELS*N_PIXELS*N_PIXELS;

int main(int argc, const char* argv[]) {
    torch::jit::script::Module model;
    try {
        model = torch::jit::load(argv[1]);
        std::cout << "Loaded the model\n";
    }
    catch (const c10::Error& e) {
        std::cerr << "error loading the model\n";
        return -1;
    }
    // Upload model to GPU
    model.to(torch::Device(torch::kXPU));
    std::cout << "Model offloaded to GPU\n\n";

    // Create the input data on the host
    std::vector<float> inputs(INPUTS_SIZE);
    srand(12345);
    for (int i=0; i<INPUTS_SIZE; i++) {
      inputs[i] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    }
    std::cout << "Generated input data on the host \n\n";

    // Move input data to the device with SYCL
    sycl::queue Q(sycl::gpu_selector_v);
    std::cout << "SYCL running on "
            << Q.get_device().get_info<sycl::info::device::name>()
            << "\n\n";
    float *d_inputs = sycl::malloc_device<float>(INPUTS_SIZE, Q);
    Q.memcpy((void *) d_inputs, (void *) inputs.data(), INPUTS_SIZE*sizeof(float));
    Q.wait();

    // Convert input array to Torch tensor
    auto options = torch::TensorOptions()
                      .dtype(torch::kFloat32)
                      .device(torch::kXPU);
    torch::Tensor input_tensor = at::from_blob(d_inputs, {N_BATCH,N_CHANNELS,N_PIXELS,N_PIXELS},
                                               nullptr, at::device(torch::kXPU).dtype(torch::kFloat32),
                                               torch::kXPU)
                                               .to(torch::kXPU);
    assert(input_tensor.dtype() == torch::kFloat32);
    assert(input_tensor.device().type() == torch::kXPU);
    std::cout << "Created the input tesor on GPU\n";

    // Perform inference
    torch::Tensor output = model.forward({input_tensor}).toTensor();
    std::cout << "Performed inference\n\n";

    std::cout << "Predicted tensor is : \n";
    std::cout << output.slice(/*dim=*/1, /*start=*/0, /*end=*/10) << '\n';

    return 0;
}
```

and the CMake commands also change to include
```
#!/bin/bash

cmake \
    -DCMAKE_CXX_FLAGS="-std=c++17 -fsycl" \
    -DCMAKE_PREFIX_PATH=`python -c 'import torch;print(torch.utils.cmake_prefix_path)'` \
    -DINTEL_EXTENSION_FOR_PYTORCH_PATH=`python -c 'import torch; print(torch.__path__[0].replace("torch","intel_extension_for_pytorch"))'` \
    ./

make
./inference-example ./resnet50_jit.pt
```

## Known Issues

* The LibTorch introspection API that are available for CUDA devices, such as `torch::cuda::is_available()`, are still under development for Intel Max 1550 GPU.







