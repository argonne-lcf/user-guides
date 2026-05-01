# GPyTorch on Aurora

## 1. Login and Queue a Job

Login to Aurora:

```bash
ssh <username>@aurora.alcf.anl.gov
```

Refer to [Getting Started on Aurora](../../getting-started-on-aurora.md) for additional information. In particular, you need to set the environment variables that provide access to the proxy host.

!!! note

    The instructions below should be **run directly from a compute node**.

    Explicitly, to request an interactive job (from `aurora-uan`):

    ```bash
    qsub -I -q <your_Queue> -l select=1,walltime=60:00 -A <your_ProjectName> -l filesystems=<fs1:fs2>
    ```

    Refer to [job scheduling and execution](../../../running-jobs/index.md) for additional information.

## 2. Once on a Compute Node, Load Modules

```bash
module load frameworks
python3 -m venv --system-site-packages env_gpytorch
source env_gpytorch/bin/activate
python3 -m pip install gpytorch
```

#### Optional

Create an `activation_env.sh` file that contains the following lines:

```bash
module load frameworks
source env_gpytorch/bin/activate
```

and run `source activation_env.sh` to activate your environment for subsequent runs.

## 3. Running on GPUs

To run on GPUs, set the device as follows in the code:

```python
if torch.cuda.is_available():
    device = torch.device('cuda')
elif torch.xpu.is_available():
    device = torch.device('xpu')
else: 
    device = torch.device('cpu')
```

### Simple example

```python linenums="1" title="gpytorch_example.py"
import torch
import gpytorch

# 1. Setup Device (will use GPU if available, else CPU)
if torch.cuda.is_available():
    device = torch.device('cuda')
elif torch.xpu.is_available():
    device = torch.device('xpu')
else:
    device = torch.device('cpu')
print(f"Using device: {device}")

# 2. Define the GP Model
class SimpleGPModel(gpytorch.models.ExactGP):
    def __init__(self, train_x, train_y, likelihood):
        super(SimpleGPModel, self).__init__(train_x, train_y, likelihood)
        self.mean_module = gpytorch.means.ConstantMean()
        self.covar_module = gpytorch.kernels.ScaleKernel(gpytorch.kernels.RBFKernel())

    def forward(self, x):
        mean_x = self.mean_module(x)
        covar_x = self.covar_module(x)
        return gpytorch.distributions.MultivariateNormal(mean_x, covar_x)

# 3. Generate synthetic data and move to device
train_x = torch.linspace(0, 1, 100).to(device)
train_y = torch.sin(train_x * (2 * 3.1415)) + torch.randn(train_x.size()).to(device) * 0.1

# 4. Initialize model/likelihood and move to device
likelihood = gpytorch.likelihoods.GaussianLikelihood().to(device)
model = SimpleGPModel(train_x, train_y, likelihood).to(device)

# 5. Training Loop
model.train()
likelihood.train()

optimizer = torch.optim.Adam(model.parameters(), lr=0.1)
mll = gpytorch.mlls.ExactMarginalLogLikelihood(likelihood, model)

print("Starting training...")
for i in range(50):
    optimizer.zero_grad()
    output = model(train_x)
    loss = -mll(output, train_y)
    loss.backward()
    optimizer.step()

    if (i + 1) % 10 == 0:
        print(f"Iter {i+1}/50 - Loss: {loss.item():.3f}")

# 6. Prediction (Inference)
model.eval()
likelihood.eval()

test_x = torch.linspace(0, 1, 50).to(device)

with torch.no_grad(), gpytorch.settings.fast_pred_var():
    observed_pred = likelihood(model(test_x))
    # Move back to CPU for standard operations like printing/plotting
    mean = observed_pred.mean.cpu()
    print(f"\nPrediction complete. Mean of first 5 points: {mean[:5]}")
```

???+ example "Output"

    ``` { .bash .no-copy }
    Using device: xpu
    Starting training...
    Iter 10/50 - Loss: 0.506
    Iter 20/50 - Loss: 0.093
    Iter 30/50 - Loss: -0.333
    Iter 40/50 - Loss: -0.633
    Iter 50/50 - Loss: -0.717
    
    Prediction complete. Mean of first 5 points: tensor([-0.0422,  0.1018,  0.2422,  0.3766,  0.5027])
    ```
    
