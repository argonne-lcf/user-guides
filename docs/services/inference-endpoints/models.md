# Available Models

Models are organized by cluster and marked with the following capabilities:

- **B** - Batch Processing Enabled
- **T** - Tool Calling Enabled
- **R** - Reasoning Enabled
- **H** - Always Hot Model

## Sophia Cluster

??? "Chat Language Models (vLLM)"

    **Meta Llama Family**

    - meta-llama/Meta-Llama-3.1-8B-Instruct^BTH^
    - meta-llama/Meta-Llama-3.1-70B-Instruct^BTH^
    - meta-llama/Meta-Llama-3.1-405B-Instruct^BT^
    - meta-llama/Llama-3.3-70B-Instruct^BT^
    - meta-llama/Llama-4-Scout-17B-16E-Instruct^BT^
    - meta-llama/Llama-4-Maverick-17B-128E-Instruct^T^

    **Mistral Family**

    - mistralai/Mistral-Large-Instruct-2407
    - mistralai/Mixtral-8x22B-Instruct-v0.1
    - mistralai/Devstral-2-123B-Instruct-2512

    **OpenAI Family**

    - openai/gpt-oss-20b^BRTH^
    - openai/gpt-oss-120b^BRTH^

    **Aurora GPT Family**

    - argonne/AuroraGPT-IT-v4-0125^B^
    - argonne/AuroraGPT-Tulu3-SFT-0125^B^
    - argonne/AuroraGPT-DPO-UFB-0225^B^
    - argonne/AuroraGPT-KTO-UFB-0325^B^

    **Google Family**

    - google/gemma-3-27b-it^BT^
    - google/gemma-4-26B-A4B-it^RT^
    - google/gemma-4-31B-it^RTH^
    - google/gemma-4-E4B-it^RTH^

    **Other Models**

    - arcee-ai/Trinity-Large-Thinking-W4A16^RT^
    - nvidia/nemotron-3-super-120b^RT^
    - AstroMLab/AstroSage-70B-20251009

??? "Vision Language Models (vLLM)"

    - meta-llama/Llama-3.2-90B-Vision-Instruct

??? "Embedding Models (vLLM)"

    - mistralai/Mistral-7B-Instruct-v0.3-embed
    - google/embeddinggemma-300m
    - Salesforce/SFR-Embedding-Mistral
    - genslm-test/genslm-esmc-300M-aminoacid
    - genslm-test/genslm-esmc-300M-codon
    - genslm-test/genslm-esmc-300M-contrastive-aminoacid
    - genslm-test/genslm-esmc-300M-contrastive-codon
    - genslm-test/genslm-esmc-300M-joint-aminoacid
    - genslm-test/genslm-esmc-300M-joint-codon
    - genslm-test/genslm-esmc-600M-aminoacid
    - genslm-test/genslm-esmc-600M-codon
    - genslm-test/genslm-esmc-600M-contrastive-aminoacid
    - genslm-test/genslm-esmc-600M-contrastive-codon
    - genslm-test/genslm-esmc-600M-joint-aminoacid
    - genslm-test/genslm-esmc-600M-joint-codon

??? "Image Segmentation"

    - sam3
    - dinov3

    !!! info "Promptable Image Segmentation Models"
        The [SAM 3](https://huggingface.co/facebook/sam3) and [DINOv3](https://github.com/facebookresearch/dinov3) models are deployed on Sophia. Install the [alcf-ai](https://pypi.org/project/alcf-ai/) package for the command line and Python toolkit. See [alcf-ai CLI and SDK](alcf-ai.md) for worked examples, including batch and DINOv3 segmentation.

        If this is your first time using `alcf-ai`, run `alcf-tokens login` first. Without a valid token, `alcf-ai` exits with an authentication error naming that command.

## Metis Cluster (SambaNova)

Endpoint and model status is on the [Metis status page](https://metis.alcf.anl.gov/status). See SambaNova's [OpenAI compatible API](https://docs.sambanova.ai/sambastudio/latest/open-ai-api.html) documentation for API details.

??? "Chat Language Models"

    - gpt-oss-120b^H^
    - Mistral-Large-3-675B-Instruct-2512^H^
    - gemma-4-31B-it^H^

    !!! note "Metis Limitations"
        - Batch processing is not currently supported on the Metis cluster.
        - Tool calling is advertised by the API, but SambaNova's sanitization of tool calls has a known issue. See the Metis Tool Calling warning under [Shims/Proxies](agents.md#shimsproxies).

## Minerva Cluster (NVIDIA)

??? "Chat Language Models"

    - nemotron-3-ultra^H^
    - inkling-bf16^H^
    - gpt-oss-120b

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

!!! note "Want to add a model?"
    To request a new model, please contact [ALCF Support](mailto:support@alcf.anl.gov?subject=Inference%20Endpoint%20Model%20Request).
