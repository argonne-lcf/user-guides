# Model Inference with OpenVINO

OpenVINO is a library developed by Intel specifically designed for accelerating inference of ML models on their CPU and GPU hardware. 
This page contains build and run instructions for Python and C/C++ examples, but please refer to the full [documentation](https://docs.openvino.ai/2023.2/home.html) for more information.



## Instlling the OpenVINO Python Runtime and CLI Tools
OpenVINO does not come with the default frameworks module on Aurora, but it can be installed manually within a virtual environment as shown below
```
module use /soft/modulefiles
module load frameworks/2023.12.15.001
python -m venv --clear /path/to/_ov_env --system-site-packages
source /path/to/_ov_env/bin/activate
pip install openvino==2023.2
pip install openvino-dev==2023.2
pip install onnx
```

Note that `/path/to/` can either be a user's home or project directory.

To use OpenVINO in the future, simply load the frameworks module and source the virtual environment.
```
module use /soft/modulefiles
module load frameworks/2023.12.15.001
source /path/to/_ov_env/bin/activate
```

## Model Converter

The first suggested step is to convert the model from one of the ML frameworks into OpenVINO's Intermediate Representation (IR). 
This consists of an `.xml` file which describes the network topology and a `.bin` file which contains the weights and biases in binary format. 
The conversion can be done from the command line with `ovc` or using the Pyrhon API `openvino.comvert_model()`.
Note that PyTorch models cannot be converted directly with `ovc` and need to be converted to ONNX format first.
You can find more information on the conversion process on OpenVINO's [documentation page](https://docs.openvino.ai/2023.2/openvino_docs_model_processing_introduction.html).

The following code snippet demonstrates how to convert the ResNet50 model from TorchVision and save the OpenVINO IR.

```
import openvino as ov
import torch
from torchvision.models import resnet50

model = resnet50(weights='DEFAULT')
input_data = torch.rand(1, 3, 224, 224)

ov_model = ov.convert_model(model, example_input=input_data)
ov.save_model(ov_model, 'resnet50.xml')
```

Information on using the CLI conversion tool can be found running `ovc -h`, which will save the model in IR format by default.

Note that by default, both `ovc` and `openvino.save_model()` perform compression of the model weights to FP16. This reduces the memory needed to store the model and can provide an increase in performance in many cases. To disable this feature, use
```
ov.save_model(ov_model, 'resnet50.xml', compress_to_fp16=False)
```

or

```
ovc </path/to/model.onnx> --compress_to_fp16=False
```

## Benchmark App

Before writing a script or program to perform inference with the OpenVINO runtime, the performance of the model can be tested with the CLI tool `benchmark_app`. 

A minimal example to run on a single PVC tile is shown below
```
benchmark_app -m resnet50.xml -hint latency -d GPU.0 -data_shape [1,3,224,224]
```

which returns a series of information on the parameters set for the benchmark tests and the performance of the tests. The last few lines of the output are shown below.

```
[ INFO ] Execution Devices:['OCL_GPU.0']
[ INFO ] Count:            6424 iterations
[ INFO ] Duration:         60011.14 ms
[ INFO ] Latency:
[ INFO ]    Median:        9.23 ms
[ INFO ]    Average:       9.25 ms
[ INFO ]    Min:           9.00 ms
[ INFO ]    Max:           11.69 ms
[ INFO ] Throughput:   107.05 FPS
```

Note that `benchmark_app` takes a number of additional configuration options as described [here](https://docs.openvino.ai/2023.2/openvino_inference_engine_tools_benchmark_tool_README.html) and running `benchmark_app -h`. 


## Inference with Python OpenVINO API

Inference can be performed invoking the compiled model directly or using the OpenVINO Runtime API explicitly to create inference requests.

An example of performing direct inference with the compiled model is shown below. 
This leads to compact code, but it performs a single synchronous inference request. 
Future calls to the model will reuse the same inference request created, thus will experience less overhead.
```
import openvino as ov
import openvino.properties.hint as hints
import torch

core = ov.Core()
config = {hints.inference_precision: 'f32'}
compiled_model = core.compile_model("resnet50.xml",device_name='GPU.0', config=config)
input_data = torch.rand((1, 3, 224, 224))
results = compiled_model(input_data)[0]
```

Note:

* The output of the direct call to the compiled model is a numpy array
* By default, OpenVINO performs inference with FP16 precision on GPU, therefore the precision type must be specified as a hint during model compilation if FP32 or other precisions are desired.

Other than the direct call to the model, the Runtime API can be used to create inference requests and control their execution.
For this approach we refer the user to the OpenVINO [documentation page](https://docs.openvino.ai/2023.2/openvino_docs_OV_UG_Integrate_OV_with_your_application.html), which clearly outlines the steps involved. 



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




