# Model Inference with OpenVINO

OpenVINO is a library developed by Intel specifically designed for accelerating inference of ML models on their CPU and GPU hardware. 
This page contains build and run instructions for Python, but please refer to the [OpenVINO GitHub page](https://github.com/openvinotoolkit/openvino) for more information.

## Installing the OpenVINO Python Runtime and CLI Tools
OpenVINO does not come with the default frameworks module on Aurora, but it can be installed manually within a Python virtual environment as shown below:
```bash
module load frameworks
python -m venv --clear /path/to/_ov_env --system-site-packages
source /path/to/_ov_env/bin/activate
pip install openvino==2024.4.0
pip install openvino-dev==2024.4.0
```

It is recommended that the path to the virtual environment be in the user's project space on [Flare](../../data-management/lustre/flare.md).

## Model Converter

The first suggested step is to convert the model from one of the ML frameworks into OpenVINO's Intermediate Representation (IR). 
This consists of an `.xml` file which describes the network topology and a `.bin` file which contains the weights and biases in binary format. 
The conversion can be done from the command line with `ovc` or using the Python API `openvino.convert_model()`.
Note that PyTorch models cannot be converted directly with `ovc` and need to be converted to ONNX format first.
You can find more information on the conversion process on [OpenVINO's documentation page](https://docs.openvino.ai/2024/openvino-workflow/model-preparation/convert-model-to-ir.html).

The following code snippet demonstrates how to use the Python API to convert the ResNet50 model from TorchVision and save the OpenVINO IR.
```python
import openvino as ov
import torch
from torchvision.models import resnet50

model = resnet50(weights='DEFAULT')
input_data = torch.rand(1, 3, 224, 224)

ov_model = ov.convert_model(model, example_input=input_data)
ov.save_model(ov_model, 'resnet50.xml')
```

Information on using the CLI conversion tool can be found by running `ovc -h`, which will save the model in IR format by default.

Note that by default, both `ovc` and `openvino.save_model()` perform compression of the model weights to FP16. This reduces the memory needed to store the model and can provide an increase in performance. 
To disable this feature, use:
```python
ov.save_model(ov_model, 'resnet50.xml', compress_to_fp16=False)
```

or

```bash
ovc model.onnx --compress_to_fp16=False
```

## Benchmark App

Before writing a script or program to perform inference with the OpenVINO runtime, the performance of the model can be tested with the CLI tool `benchmark_app`. 

A minimal example to run on a single Intel Max 1550 tile is shown below:
```bash
benchmark_app -m resnet50.xml -hint latency -d GPU.0 -data_shape [1,3,224,224]
```

which returns a series of information on the parameters set for the benchmark tests and the performance of the tests. The last few lines of the output are shown below.

``` { .bash .no-copy }
[ INFO ] Execution Devices:['GPU.0']
[ INFO ] Count:            42847 iterations
[ INFO ] Duration:         60001.96 ms
[ INFO ] Latency:
[ INFO ]    Median:        1.38 ms
[ INFO ]    Average:       1.38 ms
[ INFO ]    Min:           1.35 ms
[ INFO ]    Max:           21.31 ms
[ INFO ] Throughput:   714.09 FPS
```

Note that `benchmark_app` takes a number of additional configuration options which are listed by running `benchmark_app -h`. 

## Inference with Python OpenVINO API

Inference can be performed by invoking the compiled model directly or using the OpenVINO Runtime API explicitly to create inference requests.

An example of performing direct inference with the compiled model is shown below. 
This leads to compact code, but it performs a single synchronous inference request. 
Future calls to the model will reuse the same inference request created, thus experiencing less overhead.
```python
import openvino as ov
import torch

core = ov.Core()
compiled_model = core.compile_model("resnet50.xml")

input_data = torch.rand((1, 3, 224, 224))
results = compiled_model(input_data)[0]
```

By default, OpenVINO performs inference with FP16 precision on GPU, but the precision and device can be selected with hints, such as:
```python
import openvino.properties.hint as hints
core.set_property(
    "GPU.0",
    {hints.execution_mode: hints.ExecutionMode.ACCURACY},
)
```

More information on the available hints can be found on the [OpenVINO documentation page](https://docs.openvino.ai/2024/openvino-workflow/running-inference/optimize-inference/precision-control.html).

Other than the direct call to the model, the Runtime API can be used to create inference requests and control their execution.
For this approach, we refer the user to the [OpenVINO documentation page](https://docs.openvino.ai/2024/openvino-workflow/running-inference/integrate-openvino-with-your-application/inference-request.html).
<!---
## Inference with C++ OpenVINO API

Currently, the C++ OpenVINO API on Aurora is enabled through a pre-built set of libraries.
The environment is set as follows, with `/path/to/openvino` being a placeholder for the user to specify
```
module use /soft/modulefiles
module load spack-pe-gcc
module load cmake
export OV_PATH=/path/to/openvino
cp /home/balin/OpenVINO/SLES15.3/openvino-suse.tar.gz $OV_PATH
tar -xzvf $OV_PATH/openvino-suse.tar.gz -C $OV_PATH
source $OV_PATH/openvino/setupvars.sh
# Need to add a path to the libtbb.so.2 library needed by OpenVINO
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/soft/datascience/llm_ds/basekit_2023_0_25537/vtune/2023.0.0/lib64
export ONEAPI_DEVICE_SELECTOR=opencl:gpu
export ZE_AFFINITY_MASK=0.0
```

An example performing inference with the C++ OpenVINO API is shown below.
This simple program loads the ResNet50 model in OpenVINO IR format to the GPU (see instructions above on how to download and convert the model), creates an input vector and offloads it to the GPU with SYCL, and finally executes a single synchronous inference request on the GPU.

```c++
#include <iostream>
#include <cstdlib>
#include <vector>
#include "sycl/sycl.hpp"
#include "openvino/openvino.hpp"
#include "openvino/runtime/intel_gpu/ocl/ocl.hpp"
const int N_BATCH = 1;
const int N_CHANNELS = 3;
const int N_PIXELS = 224;
const int INPUTS_SIZE = N_BATCH*N_CHANNELS*N_PIXELS*N_PIXELS;
int main(int argc, const char* argv[])
{
  // Print some information about OpenVINO and start the runtime
  std::cout << "Running with " << ov::get_openvino_version() << "\n\n";
  ov::Core core;
  std::vector<std::string> availableDevices = core.get_available_devices();
  char device_str[] = "GPU";
  bool found_device = false;
  for (auto&& device : availableDevices) {
    if (strcmp(device.c_str(),device_str)==0) {
      std::cout << "Found device " << device << " \n\n";
      found_device = true;
    }
  }
  if (not found_device) {
    std::cout << "Input device not found \n";
    std::cout << "Available devices are: \n";
    for (auto&& device : availableDevices) {
      std::cout << device << std::endl;
    }
    return -1;
  }
  // Load the model
  std::shared_ptr<ov::Model> model = core.read_model("./resnet50.xml");
  std::cout << "Loaded model \n\n";
  // Create the input data on the host
  std::vector<float> inputs(INPUTS_SIZE);
  srand(12345);
  for (int i=0; i<INPUTS_SIZE; i++) {
    inputs[i] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
  }
  std::cout << "Generated input data on the host \n\n";
  // Move input data to the device with SYCL
  sycl::queue Q(sycl::gpu_selector_v, sycl::property::queue::in_order{}); // oneDNN needs in order queues
  std::cout << "SYCL running on "
            << Q.get_device().get_info<sycl::info::device::name>()
            << "\n\n";
  float *d_inputs = sycl::malloc_device<float>(INPUTS_SIZE, Q);
  Q.memcpy((void *) d_inputs, (void *) inputs.data(), INPUTS_SIZE*sizeof(float));
  Q.wait();
  // Share the SYCL queue and context with the GPU plugin and compile the model
  auto queue = sycl::get_native<sycl::backend::opencl>(Q);
  auto remote_context = ov::intel_gpu::ocl::ClContext(core, queue);
  auto compiled_model = core.compile_model(model, remote_context,
                                           ov::hint::inference_precision("f32"));
  // Convert input array to OpenVINO Tensor
  ov::element::Type input_type = ov::element::f32;
  ov::Shape input_shape = {N_BATCH, N_CHANNELS, N_PIXELS, N_PIXELS};
  //ov::Tensor input_tensor = ov::Tensor(input_type, input_shape, d_inputs);
  auto input_tensor = remote_context.create_tensor(input_type, input_shape, (void *) d_inputs);
  // Run inference
  ov::InferRequest infer_request = compiled_model.create_infer_request();
  infer_request.set_input_tensor(input_tensor);
  infer_request.infer();
  std::cout << "Performed inference \n\n";
  // Output the predicted Torch tensor
  ov::Tensor output_tensor = infer_request.get_output_tensor();
  std::cout << "Size of output tensor " << output_tensor.get_shape() << std::endl;
  std::cout << "Predicted tensor is : \n";
  for (int i=0; i<10; i++) {
    std::cout << output_tensor.data<float>()[i] << "\n";
  }
  std::cout << "\n";
  return 0;
}
```

To build the example program, use the `CMakeLists.txt` file below
```
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(inference_openvino_sycl_example)
find_package(OpenVINO REQUIRED COMPONENTS Runtime)
set(ov_link_libraries openvino::runtime)
add_executable(inference_openvino_sycl inference_openvino_sycl.cpp)
target_link_libraries(inference_openvino_sycl ${ov_link_libraries} -lOpenCL)
set_property(TARGET inference_openvino_sycl PROPERTY CXX_STANDARD 17)
```

and execute
```
cmake -DCMAKE_CXX_FLAGS="-std=c++17 -fsycl" ./
make
./inference_openvino_sycl
```

Note:

* OpenVINO does not currently support the Level Zero backend. OpenCL must be used instead, which can be set on Aurora with `export ONEAPI_DEVICE_SELECTOR=opencl:gpu`
* The [Remote Tensor API](https://docs.openvino.ai/2023.3/openvino_docs_OV_UG_supported_plugins_GPU_RemoteTensor_API.html) must be used to share the SYCL OpenCL context with OpenVINO
--->
