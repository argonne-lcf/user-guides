---
search:
  exclude: true
---

# Introduction of Debugging
## Initial Setups
- **Submitting an interactive job** – Save time by queuing once and running multiple times.
- **[Using VNC with a Debugger](https://www.alcf.anl.gov/support-center/using-vnc-debugger-bgq)** – when displaying an X11 client (e.g., DDT) remotely over the network, interactive response is typically slow. Using the VNC server can often help you improve the situation.

## Debugging Tools
- **[Arm Forge (formerly Allinea DDT)](https://www.arm.com/products/development-tools/server-and-hpc/forge)** – a full-featured GUI-based debugging tool supporting scalar, multi-threaded, and large-scale parallel applications.
- **ATP** – Cray’s Abnormal Termination Processing can generate a merged stack backtrace if your program dies.
- **STAT** – Generate a merged stack backtrace snapshot while your run is in progress.  Useful to diagnose deadlock.
- **lgdb** – Provides a command-line interface for debugging a parallel application with gdb (the GNU Project Debugger).

## Common Debugging Issues
- **[Determining Memory Use](http://www.alcf.anl.gov/user-guides/determining-memory-use)** – learn how to use the glibc mallinfo call to get information on used/available memory in your code.

## References
<iframe width="560" height="315" src="https://www.youtube.com/embed/fg-6yFZI25o" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
