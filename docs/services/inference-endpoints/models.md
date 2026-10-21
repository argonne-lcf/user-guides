# Available Models

Models are organized by cluster and marked with the following capabilities:

- **B** - Batch Processing Enabled
- **T** - Tool Calling Enabled
- **R** - Reasoning Enabled
- **H** - Always Hot Model

The table reports each endpoint's cluster, type, organization, API protocols, context window (in tokens), supported input modalities, streaming support, reasoning, and tool calling. A `—` indicates the value is not published for that endpoint.

| Cluster | Type | Organization | Model | API Protocols | Context Window | Input Modalities | Streaming | Reasoning | Tool Calling |
|---------|------|--------------|-------|---------------|----------------|------------------|-----------|-----------|--------------|
| Sophia | Chat | Meta | meta-llama/Meta-Llama-3.1-8B-Instruct^BTH^ | chat_completions | 32768 | text | ✓ | — | ✓ |
| Sophia | Chat | Meta | meta-llama/Meta-Llama-3.1-70B-Instruct^BTH^ | chat_completions | 16384 | text | ✓ | — | ✓ |
| Sophia | Chat | Meta | meta-llama/Meta-Llama-3.1-405B-Instruct^BT^ | chat_completions | 24576 | text | ✓ | — | ✓ |
| Sophia | Chat | Meta | meta-llama/Llama-3.3-70B-Instruct^BT^ | chat_completions | 32768 | text | ✓ | — | ✓ |
| Sophia | Chat | Meta | meta-llama/Llama-4-Scout-17B-16E-Instruct^BT^ | chat_completions | 32768 | text, image | ✓ | — | ✓ |
| Sophia | Chat | Meta | meta-llama/Llama-4-Maverick-17B-128E-Instruct^T^ | chat_completions | 16384 | text, image | ✓ | — | ✓ |
| Sophia | Chat | Mistral AI | mistralai/Mistral-Large-Instruct-2407 | chat_completions | 16384 | text | ✓ | — | — |
| Sophia | Chat | Mistral AI | mistralai/Mixtral-8x22B-Instruct-v0.1 | chat_completions | 16384 | text | ✓ | — | — |
| Sophia | Chat | Mistral AI | mistralai/Devstral-2-123B-Instruct-2512^T^ | chat_completions | 16384 | text | ✓ | — | ✓ |
| Sophia | Chat | OpenAI | openai/gpt-oss-20b^BRTH^ | chat_completions | — | text | ✓ | ✓ | ✓ |
| Sophia | Chat | OpenAI | openai/gpt-oss-120b^BRTH^ | chat_completions | 65536 | text | ✓ | ✓ | ✓ |
| Sophia | Chat | Argonne | argonne/AuroraGPT-IT-v4-0125^B^ | chat_completions | — | text | ✓ | — | — |
| Sophia | Chat | Argonne | argonne/AuroraGPT-Tulu3-SFT-0125^B^ | chat_completions | — | text | ✓ | — | — |
| Sophia | Chat | Argonne | argonne/AuroraGPT-DPO-UFB-0225^B^ | chat_completions | — | text | ✓ | — | — |
| Sophia | Chat | Argonne | argonne/AuroraGPT-KTO-UFB-0325^B^ | chat_completions | — | text | ✓ | — | — |
| Sophia | Chat | Google | google/gemma-3-27b-it^BT^ | chat_completions | 32768 | text, image | ✓ | — | ✓ |
| Sophia | Chat | Google | google/gemma-4-26B-A4B-it^RT^ | chat_completions | 262144 | text | ✓ | ✓ | ✓ |
| Sophia | Chat | Google | google/gemma-4-31B-it^RTH^ | chat_completions | 128000 | text, image, video | ✓ | ✓ | ✓ |
| Sophia | Chat | Google | google/gemma-4-E4B-it^RTH^ | chat_completions | — | text, image, video | ✓ | ✓ | ✓ |
| Sophia | Chat | Arcee AI | arcee-ai/Trinity-Large-Thinking-W4A16^RT^ | chat_completions | 131072 | text | ✓ | ✓ | ✓ |
| Sophia | Chat | NVIDIA | nvidia/nemotron-3-super-120b^RT^ | chat_completions | 262144 | text | ✓ | ✓ | ✓ |
| Sophia | Chat | NVIDIA | mgoin/Nemotron-4-340B-Instruct-hf | chat_completions | — | text | ✓ | — | — |
| Sophia | Chat | AstroMLab | AstroMLab/AstroSage-70B-20251009^T^ | chat_completions | — | text | ✓ | — | ✓ |
| Sophia | Vision | Meta | meta-llama/Llama-3.2-90B-Vision-Instruct^T^ | chat_completions | 8192 | text, image | ✓ | — | ✓ |
| Sophia | Embedding | Google | google/embeddinggemma-300m | — | — | — | — | — | — |
| Sophia | Embedding | Mistral AI | mistralai/Mistral-7B-Instruct-v0.3-embed | — | — | — | — | — | — |
| Sophia | Embedding | Salesforce | Salesforce/SFR-Embedding-Mistral | — | — | — | — | — | — |
| Sophia | Genomics | genslm | genslm-test/genslm-esmc-300M-aminoacid | — | — | — | — | — | — |
| Sophia | Genomics | genslm | genslm-test/genslm-esmc-300M-codon | — | — | — | — | — | — |
| Sophia | Genomics | genslm | genslm-test/genslm-esmc-300M-contrastive | — | — | — | — | — | — |
| Sophia | Genomics | genslm | genslm-test/genslm-esmc-300M-contrastive-2026-08 | — | — | — | — | — | — |
| Sophia | Genomics | genslm | genslm-test/genslm-esmc-300M-joint | — | — | — | — | — | — |
| Sophia | Genomics | genslm | genslm-test/genslm-esmc-600M-aminoacid | — | — | — | — | — | — |
| Sophia | Genomics | genslm | genslm-test/genslm-esmc-600M-codon | — | — | — | — | — | — |
| Sophia | Genomics | genslm | genslm-test/genslm-esmc-600M-contrastive | — | — | — | — | — | — |
| Sophia | Genomics | genslm | genslm-test/genslm-esmc-600M-contrastive-2026-08 | — | — | — | — | — | — |
| Sophia | Genomics | genslm | genslm-test/genslm-esmc-600M-joint | — | — | — | — | — | — |
| Sophia | Genomics | genslm | genslm-test/genslm-esmc-6B-contrastive-2026-08 | — | — | — | — | — | — |
| Sophia | Segmentation | Meta | facebook/sam3 | — | — | — | — | — | — |
| Sophia | Segmentation | Meta | dinov3 | — | — | — | — | — | — |
| Sophia | Triton | — | amsc-d3 | KServe gRPC | — | — | — | — | — |
| Metis | Chat | OpenAI | gpt-oss-120b^RTH^ | chat_completions, responses | 131072 | text | ✓ | ✓ | ✓ |
| Metis | Chat | Google | gemma-4-31B-it^RTH^ | chat_completions, responses | 262144 | text, image, video | ✓ | ✓ | ✓ |
| Metis | Chat | Mistral AI | Mistral-Large-3-675B-Instruct-2512^TH^ | chat_completions | 32768 | text | ✓ | — | ✓ |
| Minerva | Chat | NVIDIA | nemotron-3-ultra^RTH^ | chat_completions, responses, messages | 1048576 | text | ✓ | ✓ | ✓ |
| Minerva | Chat | Thinking Machines | inkling-bf16^RTH^ | chat_completions, responses, messages | 1048576 | text, image | ✓ | ✓ | ✓ |
| Minerva | Chat | OpenAI | gpt-oss-120b^RT^ | chat_completions, responses, messages | 131072 | text | ✓ | ✓ | ✓ |
| Minerva | Chat | Meta | muse-spark-bf16^RT^ | chat_completions, responses, messages | 1048576 | text, image | ✓ | ✓ | ✓ |
| Minerva | Chat | Reflection AI | beam-1.0-bf16^RTH^ | chat_completions, responses, messages | 1048576 | text | ✓ | ✓ | ✓ |
| Tara | Chat | OpenAI | openai/gpt-oss-20b^RT^ | chat_completions, messages, responses | 65536 | text | ✓ | ✓ | ✓ |
| Tara | Chat | NVIDIA | nemotron-3-ultra^RT^ | chat_completions | 32768 | text | ✓ | ✓ | ✓ |
| Tara | Chat | Thinking Machines | inkling-bf16^RT^ | chat_completions | 131072 | text, image, audio | ✓ | ✓ | ✓ |

For Metis, endpoint and model status is on the [Metis status page](https://metis.alcf.anl.gov/status). See SambaNova's [OpenAI compatible API](https://docs.sambanova.ai/sambastudio/latest/open-ai-api.html) documentation for API details.

/// warning | Metis Tool Calling
Tool calling is advertised by the API, but SambaNova's sanitization of tool calls has a known issue. See the Metis Tool Calling warning under [Shims/Proxies](agents.md#shimsproxies).
///

/// info | Promptable Image Segmentation Models
The [SAM 3](https://huggingface.co/facebook/sam3) and [DINOv3](https://github.com/facebookresearch/dinov3) models are deployed on Sophia. Install the [alcf-ai](https://pypi.org/project/alcf-ai/) package for the command line and Python toolkit. See [alcf-ai CLI and SDK](alcf-ai.md) for worked examples, including batch and DINOv3 segmentation.

If this is your first time using `alcf-ai`, run `alcf-tokens login inference` first. Without a valid token, `alcf-ai` exits with an authentication error naming that command.
///

## Model Serving Configuration

When available, model serving configuration details can be viewed for each cluster.

```bash
#!/bin/bash

# Get your access token
access_token=$(alcf-tokens get-token inference)

# Check serving configuration for all Sophia models
curl -X GET "https://inference-api.alcf.anl.gov/resource_server/sophia/models" \
    -H "Authorization: Bearer ${access_token}"

# Check serving configuration for a specific model (e.g., openai/gpt-oss-120b)
curl -X GET "https://inference-api.alcf.anl.gov/resource_server/sophia/models?model_id=openai/gpt-oss-120b" \
    -H "Authorization: Bearer ${access_token}"

# Check serving configuration for another cluster (e.g., Tara)
curl -X GET "https://inference-api.alcf.anl.gov/resource_server/tara/models" \
    -H "Authorization: Bearer ${access_token}"
```

### Model Capabilities

Each model includes a versioned `capabilities` object describing what the deployment supports. The current schema is version 1:

| Field | Description |
| ----- | ----------- |
| `schema_version` | Version of the capability object. |
| `api_protocols` | API protocols the model accepts. |
| `context_window_tokens` | Maximum context window, in tokens. |
| `input_modalities` | Accepted input types. |
| `streaming` | Whether streaming responses are supported. |
| `reasoning.supported` | Whether the model supports reasoning. |
| `reasoning.effort_levels` | Accepted reasoning effort values. |
| `reasoning.default_effort` | Effort applied when a request omits one. |
| `reasoning.separate_output` | Whether reasoning is returned separately as `reasoning_content` instead of inline. |
| `tool_calling.supported` | Whether the model supports tool calling. |

Optional fields are omitted when they do not apply, so clients should fall back to conservative defaults. Models served by vLLM also report `max_model_len`, `max_num_seqs`, `tool_call_parser`, and `enable_auto_tool_choice` alongside `capabilities`.

#### Possible Values

| Field | Values |
| ----- | ------ |
| `schema_version` | `1` |
| `api_protocols` | `chat_completions`, `responses`, `messages` |
| `context_window_tokens` | Any positive integer |
| `input_modalities` | `text`, `image`, `video` |
| `streaming` | `true`, `false` |
| `reasoning.supported` | `true`, `false` |
| `reasoning.effort_levels` | `none`, `minimal`, `low`, `medium`, `high`, `xhigh`, `max` |
| `reasoning.default_effort` | One of the model's `reasoning.effort_levels` |
| `reasoning.separate_output` | `true`, `false` |
| `tool_calling.supported` | `true`, `false` |

`alcf-ai ls-models <cluster>` returns the same objects and uses them to configure agent harnesses. See [alcf-ai CLI and SDK](alcf-ai.md).

/// note | Want to add a model?
To request a new model, please contact [ALCF Support](mailto:support@alcf.anl.gov?subject=Inference%20Endpoint%20Model%20Request).
///
