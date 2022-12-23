# Performance Tools

## Compile Report
After a compile, see the generated compile_report.txt.<br>
"Active PE's" is the percentage of processing elements (PEs) on the wafer that will be used by the model (compute + transmission).<br>
"Compute Utilization" is the estimated percentage of time these active PEs will be running the kernel code.<br>
The product of "Active PEs" and "Compute Utilization" is the effective wafer utilization as estimated by the compiler when the application is not I/O bound. Note: these two percentages are rounded to the nearest percent in the compile_report.txt, and e.g. can be displayed as "0%". 

```console
...$ find . -name "compile_report.txt" -exec head -n 7 {} \;
Estimated Overall Performance
-------------------------------
Samples/s:                    14724.2
Compute Samples/s:            17287.7
Transmission Samples/s:       14724.2
Active PEs:                   62%
Compute Utilization:          52%
...$
```

In this example, the wafer utilization estimate is 32 percent (0.62*0.52).<br>
There is no sharing of the wafer by different jobs, so Cerebras users should strive to maximize their jobs' use of the wafer. <br>
For more details, see the Cerebras documentation: [Compile Report](https://docs.cerebras.net/en/latest/compiler-reports/compile-report.html)<br>
Consider using multiple model replicas if the model is only filling a small part of the wafer.<br>
See
[https://docs.cerebras.net/en/latest/tensorflow-docs/multiple-models/multi-replica-data-parallel-training.html](https://docs.cerebras.net/en/latest/tensorflow-docs/multiple-models/multi-replica-data-parallel-training.html)<br>
For code samples with multireplica support, see `/software/cerebras/model_zoo/anl_shared/braggnn/tf/` and `/software/cerebras/model_zoo/modelzoo/fc_mnist/tf/`.

<!---[TODO are there other compilation artifacts in the model directory that are worth describing? What about checkpoint files (which need to be delete to rerun tests)?]--->

## Cerebras's guidance on sharding and shuffling datasets
This Cerebras document covers dataset sharding, and how to shuffle datasets.<br>
[https://docs.cerebras.net/en/latest/tensorflow-docs/tuning-tf-for-cs/best-practices-tf.html](https://docs.cerebras.net/en/latest/tensorflow-docs/tuning-tf-for-cs/best-practices-tf.html)

## Cerebras's guidance on the compiler console output
This covers output to the console (and only to the console) during compile.<br>
Search the compile console output for any WARNING lines with the substring "input_fn".<br>
[https://docs.cerebras.net/en/latest/compiler-reports/input-function-report.html](https://docs.cerebras.net/en/latest/compiler-reports/input-function-report.html)

## Cerebra's Input Analyzer
In version 1.2, Cerebras introduced the `cs_input_analyzer` script, which compiles the code, analyses the input pipeline, then suggests a slurm configuration and estimates the input performance. <br>
[https://docs.cerebras.net/en/latest/scripts-and-templates/cs-input-analyzer.html](https://docs.cerebras.net/en/latest/scripts-and-templates/cs-input-analyzer.html)
