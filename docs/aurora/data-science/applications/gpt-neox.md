# Instruction for gpt-neox on Aurora

## Overview

This document provides instructions for using gpt-neox on the Aurora system. The Aurora system is a high-performance computing environment designed to support advanced AI and machine learning workloads.

## Prerequisites

Before you begin, ensure you have the following:

- Access to the Aurora system.
- A working knowledge of gpt-neox and its dependencies.
- Necessary permissions to install and run software on Aurora.

## Installation

To install gpt-neox on Aurora, follow these steps:

1. **Load the necessary modules:**

   ```bash
   module load python/3.8
   module load cuda/11.2
   ```

2. **Clone the gpt-neox repository:**

   ```bash
   git clone https://github.com/EleutherAI/gpt-neox.git
   cd gpt-neox
   ```

3. **Install the required Python packages:**

   ```bash
   pip install -r requirements.txt
   ```

## Configuration

Configure gpt-neox by editing the `config.json` file located in the root directory of the cloned repository. Ensure that the configuration settings match the specifications of the Aurora system.

## Running gpt-neox

To run gpt-neox on Aurora, execute the following command:

```bash
python train.py --config config.json
```

Ensure that all paths and environment variables are correctly set before running the command.

## Troubleshooting

If you encounter any issues, refer to the [gpt-neox GitHub repository](https://github.com/EleutherAI/gpt-neox) for troubleshooting tips and common issues.

## Support

For additional support, contact the Aurora support team or consult the [Aurora user guide](https://www.alcf.anl.gov/support-center/aurora-sunspot).