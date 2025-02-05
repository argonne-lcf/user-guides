# GPU Monitoring

Each node on ThetaGPU hosts 8 A100 GPUs. You can see information about these GPUs via the command `nvidia-smi`.

Each GPU has 40 GB of on-GPU memory. When you run applications, you will know the GPU is in use when you see the memory increase, and the GPU Utilization will be non-zero.

You can target a specific GPU with `nvidia-smi -i 0` for the first GPU, for example.

## GPU Selection

In many application codes, you may want to specify which GPU is used. This is particularly important in node-sharing applications where each GPU is running its own code, which can be either in data-parallel model training, workflow-based throughput jobs, etc. You can control individual process launches with:

```bash
# Specify to run only on GPU 4: 
export CUDA_VISIBLE_DEVICES=4 

# Let your application see GPUs 0, 1, and 7: 
export CUDA_VISIBLE_DEVICES="0,1,7"
```

In these cases, the GPU orderings will appear as a consecutive list starting with 0.

From inside an application, many software frameworks have the ability to let you target specific GPUs, including TensorFlow and PyTorch:

- [TensorFlow](https://www.tensorflow.org/guide/gpu)
- [PyTorch](https://pytorch.org/docs/stable/cuda.html)

---