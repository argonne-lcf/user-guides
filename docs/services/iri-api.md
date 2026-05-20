# ALCF IRI API

The ALCF Facility API (IRI API) provides programmatic access to ALCF compute and filesystem resources. This page covers authentication setup and usage examples. For questions or support, please contact [ALCF Support](mailto:support@alcf.anl.gov?subject=Facility%20API%20Token). The OpenAPI specification can be found at [https://api.alcf.anl.gov/openapi.json](https://api.alcf.anl.gov/openapi.json).

!!! info "Access"
    The ALCF IRI API is available to all users who can authenticate with their ALCF credentials.

## Getting Your API Token

### 1. Setup Your Environment

Create a Python 3 virtual environment and install the [Globus SDK](https://globus-sdk-python.readthedocs.io/en/stable):

```bash
python3 -m venv venv
source venv/bin/activate
pip install globus-sdk
```

Download our auth script from [this repository](https://github.com/argonne-lcf/alcf-facility-api-token):

```bash
wget https://raw.githubusercontent.com/argonne-lcf/alcf-facility-api-token/refs/heads/main/alcf_facility_api_globus_token.py
```

### 2. Authenticate

Generate the authentication flow URL with the command below. Copy-paste the URL to your browser, authenticate with your ALCF credentials, and copy-paste the resulting authorization code in your terminal.

```bash
python alcf_facility_api_globus_token.py authenticate
```

If you have issues, make sure to logout from Globus at [app.globus.org/logout](https://app.globus.org/logout), clear your browser cache or use an incognito window, and try to re-authenticate.

### 3. Retrieve Your Access Token

You can programatically retrieve your access token either from your terminal or from Python.

=== "Bash"

    ```bash
    python alcf_facility_api_globus_token.py get_access_token
    ```

=== "Environment Variable"

    ```bash
    access_token=$(python alcf_facility_api_globus_token.py get_access_token)
    ```

=== "Python"

    ```python
    from alcf_facility_api_globus_token import get_access_token
    access_token = get_access_token()
    ```

!!! info "Token Validity"
    - Access tokens are valid for **48 hours**. The `get_access_token` command will automatically refresh your token if it has expired.
    - Refreshed tokens are authorized for up to **7 days**, after which you will need to manually reauthenticate with `python alcf_facility_api_globus_token.py authenticate`. 

## API Usage Examples

This section provides simple examples on how to interface with the API as a starting point. A complete list of input arguments, filters, and capabilities can be found on the [Swagger documentation](https://api.alcf.anl.gov/).

### 1. Status

??? "1.1. Resources"

    Query the status of all available resources.

    === "cURL"

        ```bash
        #!/bin/bash
        curl -X GET "https://api.alcf.anl.gov/api/v1/status/resources"
        ```

    === "Python"

        ```python
        import requests
        response = requests.get("https://api.alcf.anl.gov/api/v1/status/resources")
        print(response.status_code)
        print(response.json())
        ```

??? "1.2. Specific Resource"
    
    Query a specific resource (e.g. Polaris) by specifying its resource ID.

    === "cURL"

        ```bash
        #!/bin/bash
        curl -X GET "https://api.alcf.anl.gov/api/v1/status/resources/55c1c993-1124-47f9-b823-514ba3849a9a"
        ```

    === "Python"

        ```python
        import requests
        response = requests.get("https://api.alcf.anl.gov/api/v1/status/resources/55c1c993-1124-47f9-b823-514ba3849a9a")
        print(response.status_code)
        print(response.json())
        ```

??? "1.3. Facility"
    
    Query the ALCF Facility metadata.

    === "cURL"

        ```bash
        #!/bin/bash
        curl -X GET "https://api.alcf.anl.gov/api/v1/facility"
        ```

    === "Python"

        ```python
        import requests
        response = requests.get("https://api.alcf.anl.gov/api/v1/facility")
        print(response.status_code)
        print(response.json())
        ```

### 2. Compute

!!! info "Currently Supported Compute Resources"

    - Polaris
    - Crux

??? "2.1. Submit a Job"

    Submits a new job to the scheduler on the target compute resource.

    === "cURL"

        ```bash
        #!/bin/bash
        access_token=$(python alcf_facility_api_globus_token.py get_access_token)

        # Polaris
        resource_id="55c1c993-1124-47f9-b823-514ba3849a9a"

        curl -X POST "https://api.alcf.anl.gov/api/v1/compute/job/${resource_id}" \
             -H "Authorization: Bearer ${access_token}" \
             -H "Content-Type: application/json" \
             -d '{
                   "executable": "/bin/bash",
                   "arguments": ["-lc", "echo Start; sleep 10; echo End"],
                   "name": "my_job",
                   "stdout_path": "/home/<username>/logs",
                   "stderr_path": "/home/<username>/logs",
                   "resources": {
                       "node_count": 1
                   },
                   "attributes": {
                       "duration": 300,
                       "queue_name": "<system-queue-name>",
                       "account": "<your-project>",
                       "custom_attributes": {"filesystems": "home:eagle"}
                   }
                 }'
        ```

    === "Python"

        ```python
        import requests
        from alcf_facility_api_globus_token import get_access_token

        # Create headers with access token
        access_token = get_access_token()
        headers = {
            "Authorization": f"Bearer {access_token}",
            "Content-Type": "application/json"
        }

        # Polaris
        resource_id = "55c1c993-1124-47f9-b823-514ba3849a9a"

        # Equivalent to the body of a `qsub` script (excluding `#PBS` directives)
        commands = "echo Start; sleep 10; echo End"

        response = requests.post(
            f"https://api.alcf.anl.gov/api/v1/compute/job/{resource_id}",
            json={
                "executable": "/bin/bash",
                "arguments": ["-lc", commands],
                "name": "my_job",
                "stdout_path": "/home/<username>/logs",
                "stderr_path": "/home/<username>/logs",
                "resources": {
                    "node_count": 1
                },
                "attributes": {
                    "duration": 300,
                    "queue_name": "<system-queue-name>",
                    "account": "<your-project>",
                    "custom_attributes": {"filesystems": "home:eagle"}
                }
            },
            headers=headers
        )

        print(response.status_code)
        print(response.json())
        ```

??? "2.2. List Jobs"

    Returns a paginated list of jobs on the target resource. Set `historical` to `true` to include completed jobs.

    === "cURL"

        ```bash
        #!/bin/bash
        access_token=$(python alcf_facility_api_globus_token.py get_access_token)

        # Polaris
        resource_id="55c1c993-1124-47f9-b823-514ba3849a9a"

        curl -X POST "https://api.alcf.anl.gov/api/v1/compute/status/${resource_id}?historical=false&limit=10&offset=0" \
             -H "Authorization: Bearer ${access_token}" \
             -H "Content-Type: application/json"
        ```

    === "Python"

        ```python
        import requests
        from alcf_facility_api_globus_token import get_access_token

        # Create headers with access token
        access_token = get_access_token()
        headers = {
            "Authorization": f"Bearer {access_token}",
            "Content-Type": "application/json"
        }

        # Polaris
        resource_id = "55c1c993-1124-47f9-b823-514ba3849a9a"

        response = requests.post(
            f"https://api.alcf.anl.gov/api/v1/compute/status/{resource_id}",
            params={
                "historical": "false",
                "limit": 10,
                "offset": 0
            },
            headers=headers
        )

        print(response.status_code)
        print(response.json())
        ```

??? "2.3. Get a Specific Job"

    Returns the status and details of a single job by its ID.

    === "cURL"

        ```bash
        #!/bin/bash
        access_token=$(python alcf_facility_api_globus_token.py get_access_token)

        # Polaris
        resource_id="55c1c993-1124-47f9-b823-514ba3849a9a"
        job_id="<job_id>"

        curl -X GET "https://api.alcf.anl.gov/api/v1/compute/status/${resource_id}/${job_id}?historical=true" \
             -H "Authorization: Bearer ${access_token}"
        ```

    === "Python"

        ```python
        import requests
        from alcf_facility_api_globus_token import get_access_token

        # Create headers with access token
        access_token = get_access_token()
        headers = {
            "Authorization": f"Bearer {access_token}",
            "Content-Type": "application/json"
        }

        # Polaris
        resource_id = "55c1c993-1124-47f9-b823-514ba3849a9a"
        job_id = "<job_id>"

        response = requests.get(
            f"https://api.alcf.anl.gov/api/v1/compute/status/{resource_id}/{job_id}",
            params={"historical": "true"},
            headers=headers
        )

        print(response.status_code)
        print(response.json())
        ```

??? "2.4. Cancel a Job"

    Cancels a queued or running job. Returns HTTP `204 No Content` on success.

    === "cURL"

        ```bash
        #!/bin/bash
        access_token=$(python alcf_facility_api_globus_token.py get_access_token)

        # Polaris
        resource_id="55c1c993-1124-47f9-b823-514ba3849a9a"
        job_id="<job_id>"

        curl -X DELETE "https://api.alcf.anl.gov/api/v1/compute/cancel/${resource_id}/${job_id}" \
             -H "Authorization: Bearer ${access_token}"
        ```

    === "Python"

        ```python
        import requests
        from alcf_facility_api_globus_token import get_access_token

        # Create headers with access token
        access_token = get_access_token()
        headers = {
            "Authorization": f"Bearer {access_token}",
            "Content-Type": "application/json"
        }

        # Polaris
        resource_id = "55c1c993-1124-47f9-b823-514ba3849a9a"
        job_id = "<job_id>"

        response = requests.delete(
            f"https://api.alcf.anl.gov/api/v1/compute/cancel/{resource_id}/{job_id}",
            headers=headers
        )

        print(response.status_code)
        print("Job canceled." if response.status_code == 204 else response.json())
        ```

### 3. Filesystem

!!! info "Asynchronous Operations"
    All filesystem operations are asynchronous and return a task ID. See [Get a Task](#4-tasks) for how to retrieve your results.

!!! info "Currently Supported Filesystems"
    - Eagle
    - Home

??? "3.1. List Directory Contents (`ls`)"

    Returns the contents of a directory on the specified resource.

    === "cURL"

        ```bash
        #!/bin/bash
        access_token=$(python alcf_facility_api_globus_token.py get_access_token)

        # Eagle
        resource_id="1c3ad9d4-2e91-42bc-becb-72b1fde1235c"

        curl -X GET "https://api.alcf.anl.gov/api/v1/filesystem/ls/${resource_id}?path=/eagle/<your-project>" \
             -H "Authorization: Bearer ${access_token}"
        ```

    === "Python"

        ```python
        import requests
        from alcf_facility_api_globus_token import get_access_token

        # Create headers with access token
        access_token = get_access_token()
        headers = {
            "Authorization": f"Bearer {access_token}",
            "Content-Type": "application/json"
        }

        # Eagle
        resource_id = "1c3ad9d4-2e91-42bc-becb-72b1fde1235c"

        response = requests.get(
            f"https://api.alcf.anl.gov/api/v1/filesystem/ls/{resource_id}",
            params={"path": "/eagle/<your-project>"},
            headers=headers
        )

        print(response.status_code)
        print(response.json())
        ```

??? "3.2. Create a Directory (`mkdir`)"

    Creates a new directory at the specified path. Set `parent` to `True` to create any missing parent directories.

    === "cURL"

        ```bash
        #!/bin/bash
        access_token=$(python alcf_facility_api_globus_token.py get_access_token)

        # Eagle
        resource_id="1c3ad9d4-2e91-42bc-becb-72b1fde1235c"

        curl -X POST "https://api.alcf.anl.gov/api/v1/filesystem/mkdir/${resource_id}" \
             -H "Authorization: Bearer ${access_token}" \
             -H "Content-Type: application/json" \
             -d '{"path": "/eagle/<your-project>/my_new_dir", "parent": false}'
        ```

    === "Python"

        ```python
        import requests
        from alcf_facility_api_globus_token import get_access_token

        # Create headers with access token
        access_token = get_access_token()
        headers = {
            "Authorization": f"Bearer {access_token}",
            "Content-Type": "application/json"
        }

        # Eagle
        resource_id = "1c3ad9d4-2e91-42bc-becb-72b1fde1235c"

        response = requests.post(
            f"https://api.alcf.anl.gov/api/v1/filesystem/mkdir/{resource_id}",
            json={
                "path": "/eagle/<your-project>/my_new_dir",
                "parent": False
            },
            headers=headers
        )

        print(response.status_code)
        print(response.json())
        ```

??? "3.3. View File Contents (`view`)"

    Returns a portion of a file starting at a given byte `offset` and reading up to `size` bytes.

    === "cURL"

        ```bash
        #!/bin/bash
        access_token=$(python alcf_facility_api_globus_token.py get_access_token)

        # Eagle
        resource_id="1c3ad9d4-2e91-42bc-becb-72b1fde1235c"

        curl -X GET "https://api.alcf.anl.gov/api/v1/filesystem/view/${resource_id}?path=/eagle/<your-project>/file.txt&size=10&offset=0" \
             -H "Authorization: Bearer ${access_token}"
        ```

    === "Python"

        ```python
        import requests
        from alcf_facility_api_globus_token import get_access_token

        # Create headers with access token
        access_token = get_access_token()
        headers = {
            "Authorization": f"Bearer {access_token}",
            "Content-Type": "application/json"
        }

        # Eagle
        resource_id = "1c3ad9d4-2e91-42bc-becb-72b1fde1235c"

        response = requests.get(
            f"https://api.alcf.anl.gov/api/v1/filesystem/view/{resource_id}",
            params={
                "path": "/eagle/<your-project>/file.txt",
                "size": 10,
                "offset": 0
            },
            headers=headers
        )

        print(response.status_code)
        print(response.json())
        ```

??? "3.4. Read First Lines of a File (`head`)"

    Returns the first N `lines` of a file.

    === "cURL"

        ```bash
        #!/bin/bash
        access_token=$(python alcf_facility_api_globus_token.py get_access_token)

        # Eagle
        resource_id="1c3ad9d4-2e91-42bc-becb-72b1fde1235c"

        curl -X GET "https://api.alcf.anl.gov/api/v1/filesystem/head/${resource_id}?path=/eagle/<your-project>/file.txt&lines=3" \
             -H "Authorization: Bearer ${access_token}"
        ```

    === "Python"

        ```python
        import requests
        from alcf_facility_api_globus_token import get_access_token

        # Create headers with access token
        access_token = get_access_token()
        headers = {
            "Authorization": f"Bearer {access_token}",
            "Content-Type": "application/json"
        }

        # Eagle
        resource_id = "1c3ad9d4-2e91-42bc-becb-72b1fde1235c"

        response = requests.get(
            f"https://api.alcf.anl.gov/api/v1/filesystem/head/{resource_id}",
            params={
                "path": "/eagle/<your-project>/file.txt",
                "lines": 3
            },
            headers=headers
        )

        print(response.status_code)
        print(response.json())
        ```

??? "3.5. Change File Ownership (`chown`)"

    Changes the `owner` and/or `group` of a file or directory.

    === "cURL"

        ```bash
        #!/bin/bash
        access_token=$(python alcf_facility_api_globus_token.py get_access_token)

        # Eagle
        resource_id="1c3ad9d4-2e91-42bc-becb-72b1fde1235c"

        curl -X PUT "https://api.alcf.anl.gov/api/v1/filesystem/chown/${resource_id}" \
             -H "Authorization: Bearer ${access_token}" \
             -H "Content-Type: application/json" \
             -d '{"path": "/eagle/<your-project>/file.txt", "owner": "<username>", "group": "<group>"}'
        ```

    === "Python"

        ```python
        import requests
        from alcf_facility_api_globus_token import get_access_token

        # Create headers with access token
        access_token = get_access_token()
        headers = {
            "Authorization": f"Bearer {access_token}",
            "Content-Type": "application/json"
        }

        # Eagle
        resource_id = "1c3ad9d4-2e91-42bc-becb-72b1fde1235c"

        response = requests.put(
            f"https://api.alcf.anl.gov/api/v1/filesystem/chown/{resource_id}",
            json={
                "path": "/eagle/<your-project>/file.txt",
                "owner": "<username>",
                "group": "<group>"
            },
            headers=headers
        )

        print(response.status_code)
        print(response.json())
        ```

??? "3.6. Change File Permissions (`chmod`)"

    Changes the permissions of a file or directory using an octal `mode` string.

    === "cURL"

        ```bash
        #!/bin/bash
        access_token=$(python alcf_facility_api_globus_token.py get_access_token)

        # Eagle
        resource_id="1c3ad9d4-2e91-42bc-becb-72b1fde1235c"

        curl -X PUT "https://api.alcf.anl.gov/api/v1/filesystem/chmod/${resource_id}" \
             -H "Authorization: Bearer ${access_token}" \
             -H "Content-Type: application/json" \
             -d '{"path": "/eagle/<your-project>/file.txt", "mode": "700"}'
        ```

    === "Python"

        ```python
        import requests
        from alcf_facility_api_globus_token import get_access_token

        # Create headers with access token
        access_token = get_access_token()
        headers = {
            "Authorization": f"Bearer {access_token}",
            "Content-Type": "application/json"
        }

        # Eagle
        resource_id = "1c3ad9d4-2e91-42bc-becb-72b1fde1235c"

        response = requests.put(
            f"https://api.alcf.anl.gov/api/v1/filesystem/chmod/{resource_id}",
            json={
                "path": "/eagle/<your-project>/file.txt",
                "mode": "700"
            },
            headers=headers
        )

        print(response.status_code)
        print(response.json())
        ```

### 4. Tasks

??? "4.1. Get a Task"

    Retrieves the status and result of an asynchronous task by its ID.

    === "cURL"

        ```bash
        #!/bin/bash
        access_token=$(python alcf_facility_api_globus_token.py get_access_token)

        task_id="<task_id>"

        curl -X GET "https://api.alcf.anl.gov/api/v1/task/${task_id}" \
             -H "Authorization: Bearer ${access_token}"
        ```

    === "Python"

        ```python
        import requests
        from alcf_facility_api_globus_token import get_access_token

        # Create headers with access token
        access_token = get_access_token()
        headers = {
            "Authorization": f"Bearer {access_token}",
            "Content-Type": "application/json"
        }

        task_id = "<task_id>"

        response = requests.get(
            f"https://api.alcf.anl.gov/api/v1/task/{task_id}",
            headers=headers
        )

        print(response.status_code)
        print(response.json())
        ```

### 5. Account

Comming soon.


## Troubleshooting

- **Permission Denied:** Your token may have expired or you may not be authenticated with your ALCF credentials. Logout from Globus at [app.globus.org/logout](https://app.globus.org/logout), clear your browser cache or use an incognito window, and re-authenticate with `python alcf_facility_api_globus_token.py authenticate`.
- **IdentityMismatchError: Detected a change in identity:** This happens when trying to get an access token using a Globus identity that is not linked to the one you previously used. Locate your tokens file (typically at `~/.globus/app/8b84fc2d-49e9-49ea-b54d-b3a29a70cf31/alcf_facility_api_app/tokens.json`), delete it, and restart the authentication process.

## Further Information
Further information on and examples for the IRI API may be found at [https://github.com/doe-iri](https://github.com/doe-iri)

## Contact Us

For questions or support, please contact [ALCF Support](mailto:support@alcf.anl.gov?subject=Facility%20API%20Token).
