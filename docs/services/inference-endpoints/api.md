---
description: Call ALCF inference APIs for chat, Responses, Messages, streaming, reasoning, and image input.
---

# API Usage Examples

## Interactive API Reference

The service publishes the schema for its own API, which is the authoritative reference for request and response fields:

- **Swagger UI:** [https://inference-api.alcf.anl.gov/resource_server/docs](https://inference-api.alcf.anl.gov/resource_server/docs)
- **OpenAPI schema:** [https://inference-api.alcf.anl.gov/resource_server/openapi.json](https://inference-api.alcf.anl.gov/resource_server/openapi.json)

Both are publicly readable. Most endpoints shown here require a bearer token. The public status endpoints, `/resource_server/health` and `/resource_server/status`, do not. Because the schema is generated from the running service, it also lists a few endpoints that are internal to the service and not intended for direct use.

## Querying Endpoint Status

/// details | Querying Endpoint Status

You can check the status of models on the cluster and list all available endpoints programmatically.

//// tab | Check Job/Model Status
This endpoint provides information about what is currently live or queued.
```bash linenums="1"
#!/bin/bash

# Get your access token
access_token=$(alcf-tokens get-token inference)

# Check Sophia cluster status
curl -X GET "https://inference-api.alcf.anl.gov/resource_server/sophia/jobs" \
 -H "Authorization: Bearer ${access_token}"

# Check Metis cluster status
curl -X GET "https://inference-api.alcf.anl.gov/resource_server/metis/jobs" \
 -H "Authorization: Bearer ${access_token}"

# Check Minerva cluster status
curl -X GET "https://inference-api.alcf.anl.gov/resource_server/minerva/jobs" \
 -H "Authorization: Bearer ${access_token}"

# Check Tara cluster status
curl -X GET "https://inference-api.alcf.anl.gov/resource_server/tara/jobs" \
 -H "Authorization: Bearer ${access_token}"
```

///// tip | Switching Between Clusters
Replace `/sophia/` with `/metis/`, `/minerva/`, or `/tara/` in the URL.
/////
////

//// tab | List All Available Endpoints
This provides a list of all available endpoints.
```bash linenums="1"
#!/bin/bash

# Get your access token
access_token=$(alcf-tokens get-token inference)

curl -X GET "https://inference-api.alcf.anl.gov/resource_server/list-endpoints" \
 -H "Authorization: Bearer ${access_token}"
```
////
///

## Chat Completions

/// details | Chat Completions

This endpoint is used for conversational AI.

//// tab | cURL

```bash linenums="1"
#!/bin/bash
access_token=$(alcf-tokens get-token inference)

# Sophia cluster example
curl -X POST "https://inference-api.alcf.anl.gov/resource_server/sophia/vllm/v1/chat/completions" \
     -H "Authorization: Bearer ${access_token}" \
     -H "Content-Type: application/json" \
     -d '{
            "model": "meta-llama/Meta-Llama-3.1-8B-Instruct",
            "temperature": 0.2,
            "max_tokens": 150,
            "messages":[{"role": "user", "content": "What are the symptoms of diabetes?"}]
         }'

# Metis cluster example
curl -X POST "https://inference-api.alcf.anl.gov/resource_server/metis/api/v1/chat/completions" \
     -H "Authorization: Bearer ${access_token}" \
     -H "Content-Type: application/json" \
     -d '{
            "model": "gpt-oss-120b",
            "temperature": 0.2,
            "max_tokens": 150,
            "messages":[{"role": "user", "content": "What are the symptoms of diabetes?"}]
         }'

# Minerva cluster example
curl -X POST "https://inference-api.alcf.anl.gov/resource_server/minerva/api/v1/chat/completions" \
     -H "Authorization: Bearer ${access_token}" \
     -H "Content-Type: application/json" \
     -d '{
            "model": "mistral-large-4-nvfp4",
            "reasoning_effort": "low",
            "temperature": 1.0,
            "max_tokens": 1024,
            "messages":[{"role": "user", "content": "What are the symptoms of diabetes?"}]
         }'

# Tara cluster example
curl -X POST "https://inference-api.alcf.anl.gov/resource_server/tara/api/v1/chat/completions" \
     -H "Authorization: Bearer ${access_token}" \
     -H "Content-Type: application/json" \
     -d '{
            "model": "openai/gpt-oss-20b",
            "temperature": 0.2,
            "max_tokens": 150,
            "messages":[{"role": "user", "content": "What are the symptoms of diabetes?"}]
         }'
```
////

//// tab | Python (OpenAI SDK)

```python linenums="1"
from openai import OpenAI
from alcf_tokens.auth import get_access_token

access_token = get_access_token("inference")

# Sophia cluster
client = OpenAI(
    api_key=access_token,
    base_url="https://inference-api.alcf.anl.gov/resource_server/sophia/vllm/v1"
)

response = client.chat.completions.create(
    model="meta-llama/Meta-Llama-3.1-8B-Instruct",
    messages=[{"role": "user", "content": "What are the symptoms of diabetes?"}]
)
print(response.choices[0].message.content)

# Metis cluster
client_metis = OpenAI(
    api_key=access_token,
    base_url="https://inference-api.alcf.anl.gov/resource_server/metis/api/v1"
)

response = client_metis.chat.completions.create(
    model="gpt-oss-120b",
    messages=[{"role": "user", "content": "What are the symptoms of diabetes?"}]
)
print(response.choices[0].message.content)

# Minerva cluster
client_minerva = OpenAI(
    api_key=access_token,
    base_url="https://inference-api.alcf.anl.gov/resource_server/minerva/api/v1"
)

response = client_minerva.chat.completions.create(
    model="mistral-large-4-nvfp4",
    reasoning_effort="low",
    max_tokens=1024,
    messages=[{"role": "user", "content": "What are the symptoms of diabetes?"}]
)
print(response.choices[0].message.content)

# Tara cluster
client_tara = OpenAI(
    api_key=access_token,
    base_url="https://inference-api.alcf.anl.gov/resource_server/tara/api/v1"
)

response = client_tara.chat.completions.create(
    model="openai/gpt-oss-20b",
    messages=[{"role": "user", "content": "What are the symptoms of diabetes?"}]
)
print(response.choices[0].message.content)
```
////

//// tip | Switching Between Clusters
To target a different cluster, simply replace the cluster/framework portion of the URL:

- **Sophia**: `/resource_server/sophia/vllm/v1`
- **Metis**: `/resource_server/metis/api/v1`
- **Minerva**: `/resource_server/minerva/api/v1`
- **Tara**: `/resource_server/tara/api/v1`
////
///

## Reasoning and API Protocols

For Direct API models advertising all three protocols, use the appropriate native endpoint and request fields. Examples below use `low`; check the model's published [capabilities](models.md#model-capabilities) for accepted efforts and the default. Controls are model/backend-specific, so do not copy one protocol's payload unchanged to another.

| Protocol | Minerva endpoint suffix | Effort request fields | Separate reasoning and final output |
| --- | --- | --- | --- |
| Chat Completions | `/minerva/api/v1/chat/completions` | `"reasoning_effort": "low"` | Reasoning alongside `choices[0].message.content`; streaming uses reasoning and content deltas. |
| Responses | `/minerva/api/v1/responses` | `"reasoning": {"effort": "low"}` | Reasoning items and message items in `output`; final text blocks have type `output_text`. |
| Messages | `/minerva/api/v1/messages` | `"thinking": {"type": "adaptive"}, "output_config": {"effort": "low"}` | `thinking` and `text` blocks in `content`. |

When `reasoning.separate_output` is true, read final content separately from reasoning. The output-token budget includes reasoning; a very small budget can end before any final text appears. Responses uses `input` and `max_output_tokens`, rather than Chat Completions' `messages` and `max_tokens`. Messages requires `max_tokens` and uses its own message and tool schemas.

Add `"stream": true` to request server-sent events when streaming is advertised, and use `curl --no-buffer` when inspecting them. Consume the protocol's completion event rather than stopping after the first text or reasoning chunk. For tool calling, use each API's native tool schema and return tool results using that same protocol. Model tool calling does not imply support for provider-specific built-in tools such as web search.

## Vision Language Models

/// details | Vision Language Models

Use this endpoint to analyze images with text prompts.

//// tab | Python (OpenAI SDK)

```python linenums="1"
from openai import OpenAI
import base64
from alcf_tokens.auth import get_access_token

access_token = get_access_token("inference")
client = OpenAI(
    api_key=access_token,
    base_url="https://inference-api.alcf.anl.gov/resource_server/sophia/vllm/v1"
)

def encode_image(image_path):
    with open(image_path, "rb") as image_file:
        return base64.b64encode(image_file.read()).decode('utf-8')

image_path = "scientific_diagram.png" # Replace with your image
base64_image = encode_image(image_path)

response = client.chat.completions.create(
    model="meta-llama/Llama-3.2-90B-Vision-Instruct",
    messages=[
        {
            "role": "user",
            "content": [
                {"type": "text", "text": "Describe the key components in this scientific diagram"},
                {"type": "image_url", "image_url": {"url": f"data:image/png;base64,{base64_image}"}}
            ]
        }
    ],
    max_tokens=300
)
print(response.choices[0].message.content)
```
////
///

## Embeddings

/// details | Embeddings

This endpoint generates vector embeddings from text, currently supported by the `vLLM` framework.

//// tab | Python (OpenAI SDK)

```python linenums="1"
from openai import OpenAI
from alcf_tokens.auth import get_access_token

access_token = get_access_token("inference")
client = OpenAI(
    api_key=access_token,
    base_url="https://inference-api.alcf.anl.gov/resource_server/sophia/vllm/v1"
)

response = client.embeddings.create(
  model="mistralai/Mistral-7B-Instruct-v0.3-embed",
  input="The food was delicious and the waiter...",
  encoding_format="float"
)
print(response.data[0].embedding)
```
////
///

## Rate Limits

The service enforces two global request limits. Exceeding either of them returns `429 Too Many Requests`.

| Limit | Value |
| ----- | ----- |
| Requests per second (per IP address) | 50 req/s |
| Requests per second (per caller) | 20 req/s |

The per-IP limit applies before the per-caller one, and each returns its own body:

```json
{
  "error": "Rate limit exceeded",
  "message": "Too many requests from your IP address. Please try again later.",
  "retry_after": "1 second"
}
```

```json
{
  "detail": "Too many requests."
}
```

A request that exceeds a limit is rejected with `429` and a `Retry-After` header giving the number of seconds to wait.

<!-- Per-model token limits are not currently enabled.

Each model additionally applies tokens-per-minute limits, per model and per user, with a burst allowance and a cap on concurrent requests per user:

| Cluster | Model | Tokens/minute | Requests/minute | Burst tokens | Burst requests | Concurrent requests per user |
| ------- | ----- | -------------- | --------------- | ------------ | -------------- | --------------------------- |
| Sophia | `openai/gpt-oss-120b` | 64,000 | 120 | 128,000 | 10 | 5 |
| Metis | `openai/gpt-oss-120b` | 64,000 | 120 | 128,000 | 10 | 5 |
| Tara | `nemotron-3-ultra` | 262,144 | 30 | 262,144 | 2 | 1 |
| Tara | `inkling-bf16` | 262,144 | 30 | 262,144 | 2 | 1 |

Models with no entry in the table above have no token or request limit of their own. However, the global limits still apply. Token limits count the actual prompt and completion tokens of each response.

The values in effect are available per model:

```bash linenums="1"
#!/bin/bash

# Get your access token
access_token=$(alcf-tokens get-token inference)

# Check the limits published for each Sophia model
curl -X GET "https://inference-api.alcf.anl.gov/resource_server/sophia/models" \
    -H "Authorization: Bearer ${access_token}"
```

A model reports `rate_limit_token_per_minute` and `rate_limit_token_per_minute_per_user` when that limit is enabled. Exceeding a token limit returns the current usage alongside the error:

```json
{
  "error": {
    "code": "too_many_requests",
    "message": "Tokens/minute limit exceeded",
    "info": {
      "global_model_usage": 100000,
      "user_model_usage": 60000
    }
  }
}
```

Wait for the one-minute window to reset before retrying, and reduce the request size if a single request exceeds the per-user limit.
-->

For more examples, please see the [inference-endpoints GitHub repository](https://github.com/argonne-lcf/inference-endpoints).
