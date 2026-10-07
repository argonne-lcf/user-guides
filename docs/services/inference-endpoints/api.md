# API Usage Examples

## Interactive API Reference

The service publishes the schema for its own API, which is the authoritative reference for request and response fields:

- **Swagger UI:** [https://inference-api.alcf.anl.gov/resource_server/docs](https://inference-api.alcf.anl.gov/resource_server/docs)
- **OpenAPI schema:** [https://inference-api.alcf.anl.gov/resource_server/openapi.json](https://inference-api.alcf.anl.gov/resource_server/openapi.json)

Both are publicly readable. Most endpoints shown here require a bearer token. The public status endpoints, `/resource_server/health` and `/resource_server/status`, do not. Because the schema is generated from the running service, it also lists a few endpoints that are internal to the service and not intended for direct use.

## Querying Endpoint Status

??? "Querying Endpoint Status"

    You can check the status of models on the cluster and list all available endpoints programmatically.

    === "Check Job/Model Status"
        This endpoint provides information about what is currently live or queued.
        ```bash
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
        ```

        !!! tip "Switching Between Clusters"
            Replace `/sophia/` with `/metis/` or `/minerva/` in the URL.

    === "List All Available Endpoints"
        This provides a list of all available endpoints.
        ```bash
        #!/bin/bash

        # Get your access token
        access_token=$(alcf-tokens get-token inference)

        curl -X GET "https://inference-api.alcf.anl.gov/resource_server/list-endpoints" \
         -H "Authorization: Bearer ${access_token}"
        ```

## Chat Completions

??? "Chat Completions"

    This endpoint is used for conversational AI.

    === "cURL"

        ```bash
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
                    "model": "nemotron-3-ultra",
                    "temperature": 0.2,
                    "max_tokens": 150,
                    "messages":[{"role": "user", "content": "What are the symptoms of diabetes?"}]
                 }'
        ```

    === "Python (OpenAI SDK)"

        ```python
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
            model="nemotron-3-ultra",
            messages=[{"role": "user", "content": "What are the symptoms of diabetes?"}]
        )
        print(response.choices[0].message.content)
        ```

    !!! tip "Switching Between Clusters"
        To target a different cluster, simply replace the cluster/framework portion of the URL:

        - **Sophia**: `/resource_server/sophia/vllm/v1`
        - **Metis**: `/resource_server/metis/api/v1`
        - **Minerva**: `/resource_server/minerva/api/v1`

## Vision Language Models

??? "Vision Language Models"

    Use this endpoint to analyze images with text prompts.

    === "Python (OpenAI SDK)"

        ```python
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

## Embeddings

??? "Embeddings"

    This endpoint generates vector embeddings from text, currently supported by the `infinity` framework.

    === "Python (OpenAI SDK)"

        ```python
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

For more examples, please see the [inference-endpoints GitHub repository](https://github.com/argonne-lcf/inference-endpoints).
