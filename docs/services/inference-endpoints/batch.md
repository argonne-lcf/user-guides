# Batch Processing

For large-scale inference, the batch processing service allows you to submit a file with up to 150,000 requests.

!!! warning "Batch Processing Requirements"
    - You must have an active ALCF allocation.
    - Input files and output folders must be located within the `/eagle/argonne_tpc` project space or a world-readable directory.
    - Each line in the input file must be a complete [JSON request object (JSON Lines format)](https://platform.openai.com/docs/guides/batch#1-prepare-your-batch-file).
    - Only models marked with **B** support batch processing.

## Batch API Endpoints

### Create Batch

??? "Create Batch Request"

    === "cURL"
        ```bash
        #!/bin/bash

        # Get your access token
        access_token=$(alcf-tokens get-token inference)

        # Define the base URL
        base_url="https://inference-api.alcf.anl.gov/resource_server/sophia/vllm/v1/batches"

        # Submit batch request
        curl -X POST "$base_url" \
             -H "Authorization: Bearer ${access_token}" \
             -H "Content-Type: application/json" \
             -d '{
                  "model": "meta-llama/Meta-Llama-3.1-8B-Instruct",
                  "input_file": "/eagle/argonne_tpc/path/to/your/input.jsonl"
                }'

        # Submit batch request with custom output folder
        curl -X POST "$base_url" \
             -H "Authorization: Bearer ${access_token}" \
             -H "Content-Type: application/json" \
             -d '{
                  "model": "meta-llama/Meta-Llama-3.1-8B-Instruct",
                  "input_file": "/eagle/argonne_tpc/path/to/your/input.jsonl",
                  "output_folder_path": "/eagle/argonne_tpc/path/to/your/output/folder/"
                }'
        ```

    === "Python"
        ```python
        import requests
        import json
        from alcf_tokens.auth import get_access_token

        # Get your access token
        access_token = get_access_token("inference")

        # Define headers and URL
        headers = {
            'Authorization': f'Bearer {access_token}',
            'Content-Type': 'application/json'
        }
        url = "https://inference-api.alcf.anl.gov/resource_server/sophia/vllm/v1/batches"

        # Submit batch request
        data = {
            "model": "meta-llama/Meta-Llama-3.1-8B-Instruct",
            "input_file": "/eagle/argonne_tpc/path/to/your/input.jsonl",
            "output_folder_path": "/eagle/argonne_tpc/path/to/your/output/folder/"
        }

        response = requests.post(url, headers=headers, json=data)
        print(response.json())
        ```

### Retrieve Batch

??? "Retrieve Batch Metrics"

    === "cURL"
        ```bash
        #!/bin/bash

        # Get your access token
        access_token=$(alcf-tokens get-token inference)

        # Get results of specific batch
        batch_id="your-batch-id"
        curl -X GET "https://inference-api.alcf.anl.gov/resource_server/v1/batches/${batch_id}/result" \
             -H "Authorization: Bearer ${access_token}"
        ```

    === "Python"
        ```python
        import requests
        from alcf_tokens.auth import get_access_token

        # Get your access token
        access_token = get_access_token("inference")

        # Define headers and URL
        headers = {
            'Authorization': f'Bearer {access_token}'
        }
        batch_id = "your-batch-id"
        url = f"https://inference-api.alcf.anl.gov/resource_server/v1/batches/{batch_id}/result"

        # Get batch results
        response = requests.get(url, headers=headers)
        print(response.json())
        ```

    **Sample Output:**
    ```json
    {
        "results_file": "/eagle/argonne_tpc/path/to/your/output/folder/<input-file-name>_<model>_<batch-id>/<input-file-name>_<timestamp>.results.jsonl",
        "progress_file": "/eagle/argonne_tpc/path/to/your/output/folder/<input-file-name>_<model>_<batch-id>/<input-file-name>_<timestamp>.progress.json",
        "metrics": {
            "response_time": 27837.440138816833,
            "throughput_tokens_per_second": 3899.833442250346,
            "total_tokens": 108561380,
            "num_responses": 99985,
            "lines_processed": 100000
        }
    }
    ```

### List Batch

??? "List All Batches"

    === "cURL"
        ```bash
        #!/bin/bash

        # Get your access token
        access_token=$(alcf-tokens get-token inference)

        # List all batches
        curl -X GET "https://inference-api.alcf.anl.gov/resource_server/v1/batches" \
             -H "Authorization: Bearer ${access_token}"

        # Optionally filter by status (pending, running, completed, or failed)
        curl -X GET "https://inference-api.alcf.anl.gov/resource_server/v1/batches?status=completed" \
             -H "Authorization: Bearer ${access_token}"
        ```

    === "Python"
        ```python
        import requests
        from alcf_tokens.auth import get_access_token

        # Get your access token
        access_token = get_access_token("inference")

        # Define headers and URL
        headers = {
            'Authorization': f'Bearer {access_token}'
        }
        url = "https://inference-api.alcf.anl.gov/resource_server/v1/batches"

        # List all batches
        response = requests.get(url, headers=headers)
        print(response.json())

        # Optionally filter by status (pending, running, completed, or failed)
        params = {'status': 'completed'}
        response = requests.get(url, headers=headers, params=params)
        print(response.json())
        ```
    **Sample Output:**
    ```json
    [
      {
        "batch_id": "f8fa8efd-1111-476d-a0a0-111111111111",
        "cluster": "sophia",
        "created_at": "2025-02-20 18:39:58.049584+00:00",
        "framework": "vllm",
        "input_file": "/eagle/argonne_tpc/path/to/your/output/folder/chunk_a.jsonl",
        "status": "pending"
      },
      {
        "batch_id": "4b8a31b8-2222-479f-8c8c-222222222222",
        "cluster": "sophia",
        "created_at": "2025-02-20 18:40:30.882414+00:00",
        "framework": "vllm",
        "input_file": "/eagle/argonne_tpc/path/to/your/output/folder/chunk_b.jsonl",
        "status": "pending"
      }
    ]
    ```

### Batch Status

??? "Get Batch Status"

    === "cURL"
        ```bash
        #!/bin/bash

        # Get your access token
        access_token=$(alcf-tokens get-token inference)

        # Get status of specific batch
        batch_id="your-batch-id"
        curl -X GET "https://inference-api.alcf.anl.gov/resource_server/v1/batches/${batch_id}" \
             -H "Authorization: Bearer ${access_token}"
        ```

    === "Python"
        ```python
        import requests
        from alcf_tokens.auth import get_access_token

        # Get your access token
        access_token = get_access_token("inference")

        # Define headers and URL
        headers = {
            'Authorization': f'Bearer {access_token}'
        }
        batch_id = "your-batch-id"
        url = f"https://inference-api.alcf.anl.gov/resource_server/v1/batches/{batch_id}"

        # Get batch status
        response = requests.get(url, headers=headers)
        print(response.json())
        ```

    **Batch Status Codes:**

    - **pending**: The request was submitted, but the job has not started yet.
    - **running**: The job is currently running on a compute node.
    - **failed**: An error occurred. The error message is displayed when you query the result.
    - **completed**: :tada:

### Cancel Batch

??? "Cancel Submitted Batch"

    The inference team is currently developing a mechanism for users to cancel submitted batches. In the meantime, please contact us with your `batch_id` if you have a batch to cancel.

## Performance and Wait Times

- **Cold Starts:** The first query to an inactive model on Sophia may take 10-15 minutes to load.
- **Queueing:** During high demand, your request may be queued until resources are available.
- **Payload Limits:** Payloads are limited to 10MB per request and is further limited by the model's context window.

On Sophia, from the 10 nodes reserved for inference, 5 nodes are dedicated to serving popular models "hot" for immediate access. The remaining 5 nodes rotate through other models based on user requests. These dynamically loaded models will remain active for up to 24 hours and will be unloaded if not used for 2 hours.
