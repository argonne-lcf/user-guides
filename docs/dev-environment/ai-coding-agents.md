---
description: "Install AI coding agents in your home directory and connect them to an ALCF-hosted or self-hosted model, or drive ALCF jobs remotely through the IRI API."
tags:
  - AI Agents
---

# AI Coding Agents

AI coding agents such as [Claude Code](https://code.claude.com/docs/en/quickstart), [OpenAI Codex](https://learn.chatgpt.com/docs/codex/cli), [OpenCode](https://opencode.ai/), and [Pi](https://pi.dev/) are command-line tools that read your code, run commands, and edit files on your behalf. You can use them on ALCF systems, subject to the [AI Guidance](ai-guidance.md): they are allowed, but ALCF Support cannot help you set them up or debug them.

## Installing an Agent in your Home Directory

Under the [Software Policy](../policies/data-and-software-policies/software-policy.md), you may install and maintain software for your own use, and a single agent CLI fits easily in your [home directory quota](../data-management/filesystem-and-storage/index.md). The install scripts put the agent in `~/.local/bin` without root access:

```bash
curl -fsSL https://claude.ai/install.sh | bash   # Claude Code
source ~/.bashrc
claude --version
```

Install commands for the other agents are listed in [Inference Endpoints: Agents](../services/inference-endpoints/agents.md#installation).

/// warning | Run agents on compute nodes
Agents spawn many processes. Run them inside an interactive job rather than on a login node, which has a hard per-user task limit (see [Login Node Limits](index.md#login-node-limits)). Alternatively, run the agent on your own machine and let it submit jobs through the IRI API; see [Submitting jobs remotely with the IRI API](#submitting-jobs-remotely-with-the-iri-api).
///

## Choosing a model backend

An agent needs a model to talk to. Pick one of the following:

| Backend | When to use it | Setup |
| --- | --- | --- |
| **ALCF Inference Service** | The recommended option. Hosted models, nothing to run yourself. | [Inference Endpoints: Agents](../services/inference-endpoints/agents.md) (`alcf-ai agent configure <agent>`) |
| **Self-hosted model (Ollama)** | You need a specific open-weight model, or want everything on your own compute node. | [Below](#self-hosted-models-with-ollama) |
| **Commercial API** (Anthropic, OpenAI, …) | You have your own account and your data may leave ALCF. | The agent's own documentation |

Argo is not reachable from ALCF systems; see [AI Guidance](ai-guidance.md#argo-access).

/// note | API compatibility
Claude Code speaks the Anthropic Messages API. Codex, OpenCode, and Pi speak OpenAI-compatible APIs. A backend that offers only an OpenAI-compatible endpoint needs a translation shim before Claude Code can use it; see [Shims/Proxies](../services/inference-endpoints/agents.md#shimsproxies).
///

## Self-hosted models with Ollama

[Ollama](https://ollama.com/) serves open-weight models and supports a [subset of the Anthropic Messages API](https://docs.ollama.com/api/anthropic-compatibility), so Claude Code can use it directly, as can OpenAI-compatible agents. The steps below run both Ollama and the agent on the same NVIDIA GPU compute node (Polaris or Sophia).

**1. Start an interactive job** (Polaris example; use your own project and queue):

```bash
qsub -I -l select=1 -l walltime=1:00:00 -l filesystems=home:eagle -q debug -A <project>
```

**2. Install Ollama in your own space and keep models out of `/home`.** Model weights are often tens of gigabytes, so store them on a project file system:

```bash
export http_proxy=http://proxy.alcf.anl.gov:3128
export https_proxy=http://proxy.alcf.anl.gov:3128
export OLLAMA_MODELS=/eagle/<project>/$USER/ollama-models
mkdir -p "$OLLAMA_MODELS"
```

Install Ollama by following the [manual Linux install](https://docs.ollama.com/linux#manual-install) instructions, extracting the archive under your home or project directory instead of `/usr`.

**3. Start the server and pull a model that supports tool calling:**

```bash
OLLAMA_CONTEXT_LENGTH=65536 ollama serve > ollama.log 2>&1 &
ollama pull <model>
```

Coding agents need a long context window; Ollama recommends 64k tokens or more.

**4. Point Claude Code at the local server** (values from [Ollama's Claude Code guide](https://docs.ollama.com/integrations/claude-code)):

```bash
export ANTHROPIC_BASE_URL=http://localhost:11434
export ANTHROPIC_AUTH_TOKEN=ollama   # required by Claude Code, ignored by Ollama
export ANTHROPIC_API_KEY=""
claude --model <model>
```

/// warning | Keep the server local
Ollama listens on `localhost` by default. Do not expose it on a network interface: anyone who can reach the port can use it. Also note that Ollama "cloud" models (names ending in `:cloud`) run on Ollama's servers, not on your node, so your prompts and code leave ALCF.
///

## Submitting jobs remotely with the IRI API

An agent does not have to run on an ALCF system. It can run on your own machine and act on ALCF through the [ALCF IRI API](../services/iri-api.md). With your ALCF credentials, the agent can check system status, submit and monitor PBS jobs, and read their output.  This keeps the agent off the login nodes, and your local files stay on your machine unless you choose to stage them.

There are two ways to give an agent this access:

- **Let it call the REST API directly.** Get a token with [`alcf-tokens`](../services/iri-api.md#getting-your-api-token) and point the agent at the [API usage examples](../services/iri-api.md#2-compute). This works, but the agent writes a new request each time and nothing stops it from using the wrong account or asking for too many nodes.
- **Expose the API as tools through an MCP server (recommended).** A small [Model Context Protocol](https://modelcontextprotocol.io/) server wraps each IRI call as a named tool, such as `submit_job` or `get_job_state`, and checks limits in code before any request is sent.

The [Service-Enabled Science workshop materials](https://github.com/argonne-lcf/Service_Enabled_Science) include a working example in [`05_Agentic_Workflows`](https://github.com/argonne-lcf/Service_Enabled_Science/tree/main/05_Agentic_Workflows): one MCP server with tools for the ALCF documentation, Globus data transfer, and IRI job submission, already registered for Claude Code and OpenCode. To try it on your own machine:

```bash
git clone https://github.com/argonne-lcf/Service_Enabled_Science.git
cd Service_Enabled_Science
./setup.sh                    # installs uv, the Python environment, and the agents
source .venv/bin/activate

alcf-tokens login --authorize-transfer home --authorize-transfer eagle
alcf-tokens test-token iri    # expect {"ready": true, ...}

cd 05_Agentic_Workflows
claude                        # or: opencode
```

Then ask, for example, *"Is Polaris up?"* or *"Submit a one-node debug job on Polaris that runs `nvidia-smi` and show me the output."*

/// note | Set your own account and limits
The example is written for the workshop. Its guardrails in [`alcf_tools/common.py`](https://github.com/argonne-lcf/Service_Enabled_Science/blob/main/05_Agentic_Workflows/alcf_tools/common.py) allow only the `alcf_training` account, at most 2 nodes, and at most 30 minutes of walltime. Before you use it with your own allocation, change `ALLOWED_ACCOUNTS`, `EAGLE_PROJECT`, `MAX_NODES`, and `MAX_WALLTIME_SEC`, and the default `account` in `submit_job`.
///

/// warning | The agent spends your allocation
Jobs the agent submits run under your identity and are charged to your project. Keep the agent's confirmation prompts on, keep limits in the server rather than in the prompt, and treat your IRI token like a password: anyone who has it can submit jobs as you until it expires.
///

## Related pages

- [AI Guidance](ai-guidance.md): ALCF's policy and best practices for AI agents
- [Inference Endpoints: Agents](../services/inference-endpoints/agents.md): configure agents for the ALCF Inference Service
- [ALCF IRI API](../services/iri-api.md): tokens and REST examples for status, jobs, and files
- [Service-Enabled Science workshop](https://github.com/argonne-lcf/Service_Enabled_Science): hands-on examples for the IRI API, Globus, the Inference Service, and agentic workflows
- [vLLM on Aurora](../aurora/data-science/inference/vllm.md): serving models on Aurora
