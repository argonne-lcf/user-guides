# Performance Tools

## Tile Status

```bash
sntilestat
watch sntilestat
```

## Measure TFLOPs

This is an example for measuring TFLOPs for Conv2D forward pass.

```python
elif args.command == 'run':
    samba.session.run(inputs, section_types=['fwd'])
    #samba.session.run(inputs, section_types=['bckwd'])
    n_iters = 100
    forward_pass_time = []
    print("run starts")
    start_time_forward = time.time()
    for loop in range(n_iters):
        samba.session.run(inputs, section_types=['fwd'])
        #samba.session.run(inputs, section_types=['bckwd'])
        #samba.session.run(inputs, section_types=['fwd', 'bckwd'])
    end_time_forward = time.time()
    forward_pass_time.append(end_time_forward - start_time_forward)
    print("run ends")

    w_0 = (args.w + 2*args.pad_w - args.s)/args.wstride + 1
    h_0 = (args.h + 2*args.pad_h - args.r)/args.hstride + 1
    tflops = 2 * (w_0*h_0) * args.s * args.r * args.c * args.k * args.n
    tflops_forw = tflops/(sum(forward_pass_time)/n_iters/5)/(10**12) #tflops
    print(tflops)
    print(sum(forward_pass_time))
    print("tflops: %f"%tflops_forw)
    print("SN,Training,%s,Conv2d_fwd,%d,100,1,%d,%d,%d,%d,%d,%d,%d,0.0,%f,None,%f,%f,%f" % ("dtype", args.n, args.w, args.h, args.c, args.k, args.s, args.pad_w, args.wstride, (sum(forward_pass_time)/n_iters)/args.n, args.n/(sum(forward_pass_time)/n_iters), tflops_forw, (sum(forward_pass_time)/n_iters)/args.n))
```
