
ALCF consists of a single `GroqRackTM compute cluster` that provides an extensible accelerator network consisting of 9 `GroqNodeTM` [ groq-r01-gn-01 through groq-r01-gn-09 ] nodes with a rotational multi-node network topology. Each of these GroqNodes consists of 8 GroqCardTM accelerators in them with integrated chip-to-chip connections with a dragonfly multi-chip topology.

`GroqCardTM accelerator` is a dual-width, full-height, three-quarter length PCI-Express Gen4 x16 adapter that includes a single `GroqChipTM processor` with 230 MB of on-chip memory. Based on the proprietary Tensor Streaming Processor (TSP) architecture, the GroqChip processor is a low latency and high throughput single core SIMD compute engine capable of 750 TOPS (INT8) and 188 TFLOPS (FP16) @ 900 MHz that includes advanced vector and matrix mathematical acceleration units.  The GroqChip processor is deterministic, providing predictable and repeatable performance. 

The `GroqWare suite SDK` uses a API based programming model and enables users to develop, compile, and run models on the GroqCard accelerator in a host server system. The SDK uses a ONNX/MLIR enabled DAG compiler and it consists of Groq Compiler, Groq API, and utility tools like GroqView™ profiler and groq-runtime. 


<!--- The GroqRack 42U compute cluster has ---> <!--9 GroqNode servers, and --> <!--- 9 compute nodes (GroqNodes) named sequentially from groq-r01-gn-01 to groq-r01-gn-09.---> <!--and 1 redudant node (groq-r01-gn-09)--> <!---Each GroqNode has 2 AMD EPYCTM 7313 processors, a total of 1TB of DRAM, and 8 GroqCard accelerators, with integrated chip-to-chip connections. --->


For more information refer to the following links:

[GroqRack spec sheet](https://groq.com/wp-content/uploads/2022/10/GroqRack%E2%84%A2-Compute-Cluster-Product-Brief-v1.0.pdf)<br>
[GroqNode spec sheet](https://groq.com/wp-content/uploads/2022/10/GroqNode%E2%84%A2-Server-GN1-B8C-Product-Brief-v1.5.pdf)<br>
[GroqCard spec sheet](https://groq.com/wp-content/uploads/2022/10/GroqCard%E2%84%A2-Accelerator-Product-Brief-v1.5-.pdf)<br>
[GroqChip spec sheet](https://groq.com/wp-content/uploads/2022/10/GroqChip%E2%84%A2-Processor-Product-Brief-v1.5.pdf)<br>
([via](https://groq.com/docs/))
