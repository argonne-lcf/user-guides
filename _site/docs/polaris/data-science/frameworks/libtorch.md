# LibTorch C++ Library

LibTorch is a C++ library for Torch, with many of the APIs that are available in PyTorch. Users can find more information in the [PyTorch documentation](https://pytorch.org/cppdocs/installing.html). This is useful for integrating the Torch ML framework into traditional HPC simulation codes and therefore enables training and inference of ML models. During compilation, Intel optimizations will be activated automatically once the IPEX dynamic library is linked.

## Environment Setup

To use LibTorch on Polaris, load the ML frameworks module:
```bash
module use /soft/modulefiles
module load conda/2024-04-29
conda activate
```
This will also load `PrgEnv-gnu/8.5.0` and `cmake`.

## Torch Libraries

With the ML frameworks module loaded as shown above, run:
```bash
python -c 'import torch; print(torch.__path__[0])'
python -c 'import torch; print(torch.utils.cmake_prefix_path)'
```
to find the path to the Torch libraries, include files, and CMake files.

## Linking the Torch Libraries

When using the CMake build system, the LibTorch libraries can be linked to an example C++ application using the following `CMakeLists.txt` file:
```bash
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
cmake_policy(SET CMP0074 NEW)
project(project-name)

find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS} -Wl,--no-as-needed")
set(TORCH_LIBS ${TORCH_LIBRARIES})

add_executable(exe main.cpp)
target_link_libraries(exe ${TORCH_LIBS})

set_property(TARGET exe PROPERTY CXX_STANDARD 17)
```

and configuring the build with:
```bash
cmake \
    -DCMAKE_PREFIX_PATH=`python -c 'import torch; print(torch.utils.cmake_prefix_path)'` \
    ./
make
```

## Device Introspection

Similar to PyTorch, LibTorch provides APIs to perform introspection on the devices available on the system. The simple code below shows how to check if CUDA devices are available, how many are present, and how to loop through them to discover some properties.

```c++
#include <torch/torch.h>

int main(int argc, const char* argv[])
{
  torch::DeviceType device;
  int num_devices = 0;
  if (torch::cuda::is_available()) {
    std::cout << "CUDA devices detected" << std::endl;
    device = torch::kCUDA;

    num_devices = torch::cuda::device_count();
    std::cout << "Number of CUDA devices: " << num_devices << std::endl;
  } else {
    device = torch::kCPU;
    std::cout << "No CUDA devices detected, setting device to CPU" << std::endl;
  }

  return 0;
}
```

## Model Inferencing Using the Torch API 

This example shows how to perform inference with the ResNet50 model using LibTorch. First, get a JIT-traced version of the model by executing `python resnet50_trace.py` (shown below) on a compute node.
```python
import torch
import torchvision
from time import perf_counter

device = 'cuda'

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

Then, build `inference-example.cpp` (shown below):
```c++
#include <torch/torch.h>
#include <torch/script.h>

int main(int argc, const char* argv[]) {
  torch::jit::script::Module model;
  try {
    model = torch::jit::load(argv[1]);
    std::cout << "Loaded the model\n";
  }
  catch (const c10::Error& e) {
    std::cerr << "Error loading the model\n";
    return -1;
  }

  model.to(torch::Device(torch::kCUDA));
  std::cout << "Model offloaded to GPU\n\n";

  auto options = torch::TensorOptions()
                      .dtype(torch::kFloat32)
                      .device(torch::kCUDA);
  torch::Tensor input_tensor = torch::rand({1,3,224,224}, options);
  assert(input_tensor.dtype() == torch::kFloat32);
  assert(input_tensor.device().type() == torch::kCUDA);
  std::cout << "Created the input tensor on GPU\n";

  torch::Tensor output = model.forward({input_tensor}).toTensor();
  std::cout << "Performed inference\n\n";

  std::cout << "Slice of predicted tensor is : \n";
  std::cout << output.slice(/*dim=*/1, /*start=*/0, /*end=*/10) << '\n';

  return 0;
}
```

and execute it with `./inference-example ./resnet50_jit.pt`.