# ALCF IRI API

The ALCF Facility API (IRI API) provides programmatic access to ALCF compute and filesystem resources. This page covers authentication setup and usage examples. For questions or support, please contact [ALCF Support](mailto:support@alcf.anl.gov?subject=Facility%20API). The OpenAPI specification can be found at [https://api.alcf.anl.gov/openapi.json](https://api.alcf.anl.gov/openapi.json).

/// info | Access
The ALCF IRI API is available to all users who can authenticate with their ALCF credentials.
///

## Getting Your API Token

### 1. Setup Your Environment

/// tab | alcf-tokens

Create a Python (>=3.10) environment with `uv`, `venv`, or `conda`. See [Python Environments](../dev-environment/python-environments.md) for the options. Then install [alcf-tokens](https://pypi.org/project/alcf-tokens/):

```bash
pip install alcf-tokens
```
///

/// tab | Auth script (Deprecated)

```bash
pip install globus-sdk

wget https://raw.githubusercontent.com/argonne-lcf/alcf-facility-api-token/refs/heads/main/alcf_facility_api_globus_token.py
# If `wget` is unavailable on your system, try `curl -O` instead.
```
///

### 2. Authenticate

Generate the authentication flow URL with the command below. Copy-paste the URL to your browser, authenticate with your ALCF credentials, and copy-paste the resulting authorization code in your terminal.

/// tab | alcf-tokens

```bash
alcf-tokens login iri
```
///

/// tab | Auth script (Deprecated)

```bash
python alcf_facility_api_globus_token.py authenticate
```
///

Test your token with the following:
```bash
alcf-tokens test-token iri
```

If your token is authorized to use the IRI API, you should see `"ready": true`. If you have issues, make sure to logout from Globus at [app.globus.org/logout](https://app.globus.org/logout), clear your browser cache or use an incognito window, and try to re-authenticate.

### 3. Retrieve Your Access Token

You can programmatically retrieve your access token either from your terminal or from Python.

/// tab | alcf-tokens

```bash
# Shell
access_token=$(alcf-tokens get-token iri)
```

```python
# Python
from alcf_tokens.auth import get_access_token
access_token = get_access_token("iri")
```
///

/// tab | Auth script (Deprecated)

```bash
# Shell
access_token=$(python alcf_facility_api_globus_token.py get_access_token)
```

```python
# Python
from alcf_facility_api_globus_token import get_access_token
access_token = get_access_token()
```
///

/// info | Token Validity
- Access tokens are valid for **48 hours**. The token retrieval commands will automatically refresh your token if it has expired.
- Refreshed tokens are authorized for up to **7 days**, after which you will need to manually re-authenticate. 
///

## API Usage Examples

This section provides simple examples on how to interface with the API as a starting point. A complete list of input arguments, filters, and capabilities can be found on the [Swagger documentation](https://api.alcf.anl.gov/).

/// tip | Rendering
If available on your machine, you can add `| jq` at the end of your cURL commands to pretty-print the JSON output.
///

### 1. Status

/// details | 1.1. Resources

Query the status of all available resources.

//// tab | cURL

```bash
#!/bin/bash
curl -X GET "https://api.alcf.anl.gov/api/v1/status/resources"
```
////

//// tab | Python

```python
import requests
response = requests.get("https://api.alcf.anl.gov/api/v1/status/resources")
print(response.status_code)
print(response.json())
```
////
///

/// details | 1.2. Specific Resource

Query a specific resource (e.g. Polaris) by specifying its resource ID.

//// tab | cURL

```bash
#!/bin/bash
curl -X GET "https://api.alcf.anl.gov/api/v1/status/resources/55c1c993-1124-47f9-b823-514ba3849a9a"
```
////

//// tab | Python

```python
import requests
response = requests.get("https://api.alcf.anl.gov/api/v1/status/resources/55c1c993-1124-47f9-b823-514ba3849a9a")
print(response.status_code)
print(response.json())
```
////
///

/// details | 1.3. Facility

Query the ALCF Facility metadata.

//// tab | cURL

```bash
#!/bin/bash
curl -X GET "https://api.alcf.anl.gov/api/v1/facility"
```
////

//// tab | Python

```python
import requests
response = requests.get("https://api.alcf.anl.gov/api/v1/facility")
print(response.status_code)
print(response.json())
```
////
///

### 2. Compute

/// info | Currently Supported Compute Resources

- Aurora
- Polaris
- Crux
///

/// details | 2.1. Submit a Job

Submits a new job to the scheduler on the target compute resource.

//// info | For Aurora, use `"custom_attributes": {"filesystems": "flare"}`
////

//// tab | cURL

```bash
#!/bin/bash
access_token=$(alcf-tokens get-token iri)

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
               "queue_name": "<queue>",
               "account": "<project>",
               "custom_attributes": {"filesystems": "home:eagle"}
           }
         }'
```
////

//// tab | Python

```python
import requests
from alcf_tokens.auth import get_access_token

# Create headers with access token
access_token = get_access_token("iri")
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
            "queue_name": "<queue>",
            "account": "<project>",
            "custom_attributes": {"filesystems": "home:eagle"}
        }
    },
    headers=headers
)

print(response.status_code)
print(response.json())
```
////
///

/// details | 2.2. List Jobs

Returns a paginated list of jobs on the target resource. Set `historical` to `true` to include completed jobs.

//// tab | cURL

```bash
#!/bin/bash
access_token=$(alcf-tokens get-token iri)

# Polaris
resource_id="55c1c993-1124-47f9-b823-514ba3849a9a"

curl -X POST "https://api.alcf.anl.gov/api/v1/compute/status/${resource_id}?historical=false&limit=10&offset=0" \
     -H "Authorization: Bearer ${access_token}" \
     -H "Content-Type: application/json"
```
////

//// tab | Python

```python
import requests
from alcf_tokens.auth import get_access_token

# Create headers with access token
access_token = get_access_token("iri")
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
////

You can filter results by adding a JSON body (e.g., `{"states": ["active"]}`) to the request:

//// tab | cURL

```bash
# Filter by job state (e.g., active, queued, completed)
curl -X POST "https://api.alcf.anl.gov/api/v1/compute/status/${resource_id}?historical=true&limit=10&offset=0" \
     -H "Authorization: Bearer ${access_token}" \
     -H "Content-Type: application/json" \
     -d '{"states": ["active"]}'
```
////

//// tab | Python

```python
# Filter by job state
response = requests.post(
    f"https://api.alcf.anl.gov/api/v1/compute/status/{resource_id}",
    params={"historical": "true", "limit": 10, "offset": 0},
    json={"states": ["active"]},
    headers=headers
)
```
////

Available filters are: 

- **states**: List of job states (new, queued, held, active, completed, failed, canceled)
    - Example: `{"states": ["active", "completed"]}`
- **owner**: ALCF username
    - Example: `{"owner": "<username>"}`
- **jobIds**: List of job IDs
    - Example: `{"jobIds": ["12345", "12346", "12347"]}` 
- **queue**: Name of the PBS queue
    - Example: `{"queue": "debug"}`
- **accountingId**: Name of the compute allocation
    - Example: `{"accountingId": "<project>"}`

//// info | Combining Filters
More than one filter can be added to the same request body:

- Example: `{"states": ["active"], "queue": "debug"}`
////
///

/// details | 2.3. Get a Specific Job

Returns the status and details of a single job by its ID.

//// tab | cURL

```bash
#!/bin/bash
access_token=$(alcf-tokens get-token iri)

# Polaris
resource_id="55c1c993-1124-47f9-b823-514ba3849a9a"
job_id="<jobid>"

curl -X GET "https://api.alcf.anl.gov/api/v1/compute/status/${resource_id}/${job_id}?historical=true" \
     -H "Authorization: Bearer ${access_token}"
```
////

//// tab | Python

```python
import requests
from alcf_tokens.auth import get_access_token

# Create headers with access token
access_token = get_access_token("iri")
headers = {
    "Authorization": f"Bearer {access_token}",
    "Content-Type": "application/json"
}

# Polaris
resource_id = "55c1c993-1124-47f9-b823-514ba3849a9a"
job_id = "<jobid>"

response = requests.get(
    f"https://api.alcf.anl.gov/api/v1/compute/status/{resource_id}/{job_id}",
    params={"historical": "true"},
    headers=headers
)

print(response.status_code)
print(response.json())
```
////
///

/// details | 2.4. Cancel a Job

Cancels a queued or running job. Returns HTTP `204 No Content` on success.

//// tab | cURL

```bash
#!/bin/bash
access_token=$(alcf-tokens get-token iri)

# Polaris
resource_id="55c1c993-1124-47f9-b823-514ba3849a9a"
job_id="<jobid>"

curl -X DELETE "https://api.alcf.anl.gov/api/v1/compute/cancel/${resource_id}/${job_id}" \
     -H "Authorization: Bearer ${access_token}"
```
////

//// tab | Python

```python
import requests
from alcf_tokens.auth import get_access_token

# Create headers with access token
access_token = get_access_token("iri")
headers = {
    "Authorization": f"Bearer {access_token}",
    "Content-Type": "application/json"
}

# Polaris
resource_id = "55c1c993-1124-47f9-b823-514ba3849a9a"
job_id = "<jobid>"

response = requests.delete(
    f"https://api.alcf.anl.gov/api/v1/compute/cancel/{resource_id}/{job_id}",
    headers=headers
)

print(response.status_code)
print("Job canceled." if response.status_code == 204 else response.json())
```
////
///

### 3. Filesystem

/// info | Restricted Access (temporary)
Access to filesystem operations is currently restricted to Sophia and Aurora users. We are working on broadening the access to all ALCF users.
///

/// info | Asynchronous Operations
All filesystem operations are asynchronous and return a task ID. See [Get a Task](#4-tasks) for how to retrieve your results.
///

/// info | Currently Supported Filesystems
- Flare (All paths must start with `/flare` or `/lus/flare/projects`) 
- Eagle (All paths must start with `/eagle` or `/lus/eagle`) 
- Home (All paths must start with `/home`)
///

/// details | 3.1. List Directory Contents (`ls`)

Returns the contents of a directory on the specified resource.

//// tab | cURL

```bash
#!/bin/bash
access_token=$(alcf-tokens get-token iri)

# Eagle
resource_id="1c3ad9d4-2e91-42bc-becb-72b1fde1235c"

curl -X GET "https://api.alcf.anl.gov/api/v1/filesystem/ls/${resource_id}?path=/eagle/<project>" \
     -H "Authorization: Bearer ${access_token}"
```
////

//// tab | Python

```python
import requests
from alcf_tokens.auth import get_access_token

# Create headers with access token
access_token = get_access_token("iri")
headers = {
    "Authorization": f"Bearer {access_token}",
    "Content-Type": "application/json"
}

# Eagle
resource_id = "1c3ad9d4-2e91-42bc-becb-72b1fde1235c"

response = requests.get(
    f"https://api.alcf.anl.gov/api/v1/filesystem/ls/{resource_id}",
    params={"path": "/eagle/<project>"},
    headers=headers
)

print(response.status_code)
print(response.json())
```
////
///

/// details | 3.2. Create a Directory (`mkdir`)

Creates a new directory at the specified path. Set `parent` to `True` to create any missing parent directories.

//// tab | cURL

```bash
#!/bin/bash
access_token=$(alcf-tokens get-token iri)

# Eagle
resource_id="1c3ad9d4-2e91-42bc-becb-72b1fde1235c"

curl -X POST "https://api.alcf.anl.gov/api/v1/filesystem/mkdir/${resource_id}" \
     -H "Authorization: Bearer ${access_token}" \
     -H "Content-Type: application/json" \
     -d '{"path": "/eagle/<project>/my_new_dir", "parent": false}'
```
////

//// tab | Python

```python
import requests
from alcf_tokens.auth import get_access_token

# Create headers with access token
access_token = get_access_token("iri")
headers = {
    "Authorization": f"Bearer {access_token}",
    "Content-Type": "application/json"
}

# Eagle
resource_id = "1c3ad9d4-2e91-42bc-becb-72b1fde1235c"

response = requests.post(
    f"https://api.alcf.anl.gov/api/v1/filesystem/mkdir/{resource_id}",
    json={
        "path": "/eagle/<project>/my_new_dir",
        "parent": False
    },
    headers=headers
)

print(response.status_code)
print(response.json())
```
////
///

/// details | 3.3. View File Contents (`view`)

Returns a portion of a file starting at a given byte `offset` and reading up to `size` bytes.

//// tab | cURL

```bash
#!/bin/bash
access_token=$(alcf-tokens get-token iri)

# Eagle
resource_id="1c3ad9d4-2e91-42bc-becb-72b1fde1235c"

curl -X GET "https://api.alcf.anl.gov/api/v1/filesystem/view/${resource_id}?path=/eagle/<project>/file.txt&size=10&offset=0" \
     -H "Authorization: Bearer ${access_token}"
```
////

//// tab | Python

```python
import requests
from alcf_tokens.auth import get_access_token

# Create headers with access token
access_token = get_access_token("iri")
headers = {
    "Authorization": f"Bearer {access_token}",
    "Content-Type": "application/json"
}

# Eagle
resource_id = "1c3ad9d4-2e91-42bc-becb-72b1fde1235c"

response = requests.get(
    f"https://api.alcf.anl.gov/api/v1/filesystem/view/{resource_id}",
    params={
        "path": "/eagle/<project>/file.txt",
        "size": 10,
        "offset": 0
    },
    headers=headers
)

print(response.status_code)
print(response.json())
```
////
///

/// details | 3.4. Read First Lines of a File (`head`)

Returns the first N `lines` of a file.

//// tab | cURL

```bash
#!/bin/bash
access_token=$(alcf-tokens get-token iri)

# Eagle
resource_id="1c3ad9d4-2e91-42bc-becb-72b1fde1235c"

curl -X GET "https://api.alcf.anl.gov/api/v1/filesystem/head/${resource_id}?path=/eagle/<project>/file.txt&lines=3" \
     -H "Authorization: Bearer ${access_token}"
```
////

//// tab | Python

```python
import requests
from alcf_tokens.auth import get_access_token

# Create headers with access token
access_token = get_access_token("iri")
headers = {
    "Authorization": f"Bearer {access_token}",
    "Content-Type": "application/json"
}

# Eagle
resource_id = "1c3ad9d4-2e91-42bc-becb-72b1fde1235c"

response = requests.get(
    f"https://api.alcf.anl.gov/api/v1/filesystem/head/{resource_id}",
    params={
        "path": "/eagle/<project>/file.txt",
        "lines": 3
    },
    headers=headers
)

print(response.status_code)
print(response.json())
```
////
///

/// details | 3.5. Read Last Lines of a File (`tail`)

Returns the last N `lines` of a file.

//// tab | cURL

```bash
#!/bin/bash
access_token=$(alcf-tokens get-token iri)

# Eagle
resource_id="1c3ad9d4-2e91-42bc-becb-72b1fde1235c"

curl -X GET "https://api.alcf.anl.gov/api/v1/filesystem/tail/${resource_id}?path=/eagle/<project>/file.txt&lines=3" \
     -H "Authorization: Bearer ${access_token}"
```
////

//// tab | Python

```python
import requests
from alcf_tokens.auth import get_access_token

# Create headers with access token
access_token = get_access_token("iri")
headers = {
    "Authorization": f"Bearer {access_token}",
    "Content-Type": "application/json"
}

# Eagle
resource_id = "1c3ad9d4-2e91-42bc-becb-72b1fde1235c"

response = requests.get(
    f"https://api.alcf.anl.gov/api/v1/filesystem/tail/{resource_id}",
    params={
        "path": "/eagle/<project>/file.txt",
        "lines": 3
    },
    headers=headers
)

print(response.status_code)
print(response.json())
```
////
///

/// details | 3.6. Get File Checksum (`checksum`)

Returns the SHA-256 checksum of a file.

//// tab | cURL

```bash
#!/bin/bash
access_token=$(alcf-tokens get-token iri)

# Eagle
resource_id="1c3ad9d4-2e91-42bc-becb-72b1fde1235c"

curl -X GET "https://api.alcf.anl.gov/api/v1/filesystem/checksum/${resource_id}?path=/eagle/<project>/file.txt" \
     -H "Authorization: Bearer ${access_token}"
```
////

//// tab | Python

```python
import requests
from alcf_tokens.auth import get_access_token

# Create headers with access token
access_token = get_access_token("iri")
headers = {
    "Authorization": f"Bearer {access_token}",
    "Content-Type": "application/json"
}

# Eagle
resource_id = "1c3ad9d4-2e91-42bc-becb-72b1fde1235c"

response = requests.get(
    f"https://api.alcf.anl.gov/api/v1/filesystem/checksum/{resource_id}",
    params={"path": "/eagle/<project>/file.txt"},
    headers=headers
)

print(response.status_code)
print(response.json())
```
////
///

/// details | 3.7. Get File Type (`file`)

Returns the type of a file or directory.

//// tab | cURL

```bash
#!/bin/bash
access_token=$(alcf-tokens get-token iri)

# Eagle
resource_id="1c3ad9d4-2e91-42bc-becb-72b1fde1235c"

curl -X GET "https://api.alcf.anl.gov/api/v1/filesystem/file/${resource_id}?path=/eagle/<project>/file.txt" \
     -H "Authorization: Bearer ${access_token}"
```
////

//// tab | Python

```python
import requests
from alcf_tokens.auth import get_access_token

# Create headers with access token
access_token = get_access_token("iri")
headers = {
    "Authorization": f"Bearer {access_token}",
    "Content-Type": "application/json"
}

# Eagle
resource_id = "1c3ad9d4-2e91-42bc-becb-72b1fde1235c"

response = requests.get(
    f"https://api.alcf.anl.gov/api/v1/filesystem/file/{resource_id}",
    params={"path": "/eagle/<project>/file.txt"},
    headers=headers
)

print(response.status_code)
print(response.json())
```
////
///

/// details | 3.8. Change File Ownership (`chown`)

Changes the `owner` and/or `group` of a file or directory.

//// tab | cURL

```bash
#!/bin/bash
access_token=$(alcf-tokens get-token iri)

# Eagle
resource_id="1c3ad9d4-2e91-42bc-becb-72b1fde1235c"

curl -X PUT "https://api.alcf.anl.gov/api/v1/filesystem/chown/${resource_id}" \
     -H "Authorization: Bearer ${access_token}" \
     -H "Content-Type: application/json" \
     -d '{"path": "/eagle/<project>/file.txt", "owner": "<username>", "group": "<group>"}'
```
////

//// tab | Python

```python
import requests
from alcf_tokens.auth import get_access_token

# Create headers with access token
access_token = get_access_token("iri")
headers = {
    "Authorization": f"Bearer {access_token}",
    "Content-Type": "application/json"
}

# Eagle
resource_id = "1c3ad9d4-2e91-42bc-becb-72b1fde1235c"

response = requests.put(
    f"https://api.alcf.anl.gov/api/v1/filesystem/chown/{resource_id}",
    json={
        "path": "/eagle/<project>/file.txt",
        "owner": "<username>",
        "group": "<group>"
    },
    headers=headers
)

print(response.status_code)
print(response.json())
```
////
///

/// details | 3.9. Change File Permissions (`chmod`)

Changes the permissions of a file or directory using an octal `mode` string.

//// tab | cURL

```bash
#!/bin/bash
access_token=$(alcf-tokens get-token iri)

# Eagle
resource_id="1c3ad9d4-2e91-42bc-becb-72b1fde1235c"

curl -X PUT "https://api.alcf.anl.gov/api/v1/filesystem/chmod/${resource_id}" \
     -H "Authorization: Bearer ${access_token}" \
     -H "Content-Type: application/json" \
     -d '{"path": "/eagle/<project>/file.txt", "mode": "700"}'
```
////

//// tab | Python

```python
import requests
from alcf_tokens.auth import get_access_token

# Create headers with access token
access_token = get_access_token("iri")
headers = {
    "Authorization": f"Bearer {access_token}",
    "Content-Type": "application/json"
}

# Eagle
resource_id = "1c3ad9d4-2e91-42bc-becb-72b1fde1235c"

response = requests.put(
    f"https://api.alcf.anl.gov/api/v1/filesystem/chmod/{resource_id}",
    json={
        "path": "/eagle/<project>/file.txt",
        "mode": "700"
    },
    headers=headers
)

print(response.status_code)
print(response.json())
```
////
///

/// details | 3.10. Remove File or Directory (`rm`)

Delete file or directory given a specific path.

//// tab | cURL

```bash
#!/bin/bash
access_token=$(alcf-tokens get-token iri)

# Home
resource_id="6115bd2c-957a-4543-abff-5fae52992ff2"

curl -X DELETE "https://api.alcf.anl.gov/api/v1/filesystem/rm/${resource_id}?path=/home/<username>/my_dir" \
     -H "Authorization: Bearer ${access_token}"
```
////

//// tab | Python

```python
import requests
from alcf_tokens.auth import get_access_token

# Create headers with access token
access_token = get_access_token("iri")
headers = {
    "Authorization": f"Bearer {access_token}",
    "Content-Type": "application/json"
}

# Home
resource_id = "6115bd2c-957a-4543-abff-5fae52992ff2"

response = requests.delete(
    f"https://api.alcf.anl.gov/api/v1/filesystem/rm/{resource_id}",
    params={"path": "/home/<username>/my_dir"},
    headers=headers
)

print(response.status_code)
print(response.json())
```
////
///

### 4. Tasks

/// details | 4.1. Get a Task

Retrieves the status and result of an asynchronous task by its ID.

//// tab | cURL

```bash
#!/bin/bash
access_token=$(alcf-tokens get-token iri)

task_id="<task_id>"

curl -X GET "https://api.alcf.anl.gov/api/v1/task/${task_id}" \
     -H "Authorization: Bearer ${access_token}"
```
////

//// tab | Python

```python
import requests
from alcf_tokens.auth import get_access_token

# Create headers with access token
access_token = get_access_token("iri")
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
////
///

### 5. Account

/// details | 5.1. List Projects

Returns the list of projects associated with your account.

//// tab | cURL

```bash
#!/bin/bash
access_token=$(alcf-tokens get-token iri)

curl -X GET "https://api.alcf.anl.gov/api/v1/account/projects" \
     -H "Authorization: Bearer ${access_token}"
```
////

//// tab | Python

```python
import requests
from alcf_tokens.auth import get_access_token

# Create headers with access token
access_token = get_access_token("iri")
headers = {
    "Authorization": f"Bearer {access_token}",
    "Content-Type": "application/json"
}

response = requests.get(
    "https://api.alcf.anl.gov/api/v1/account/projects",
    headers=headers
)

print(response.status_code)
print(response.json())
```
////
///

/// details | 5.2. Get a Specific Project

Returns a specific project by its ID.

//// tab | cURL

```bash
#!/bin/bash
access_token=$(alcf-tokens get-token iri)

project_id="<project_id>"

curl -X GET "https://api.alcf.anl.gov/api/v1/account/projects/${project_id}" \
     -H "Authorization: Bearer ${access_token}"
```
////

//// tab | Python

```python
import requests
from alcf_tokens.auth import get_access_token

# Create headers with access token
access_token = get_access_token("iri")
headers = {
    "Authorization": f"Bearer {access_token}",
    "Content-Type": "application/json"
}

project_id = "<project_id>"

response = requests.get(
    f"https://api.alcf.anl.gov/api/v1/account/projects/{project_id}",
    headers=headers
)

print(response.status_code)
print(response.json())
```
////
///

/// details | 5.3. List Project Allocations

Returns the list of allocations for a specific project.

//// tab | cURL

```bash
#!/bin/bash
access_token=$(alcf-tokens get-token iri)

project_id="<project_id>"

curl -X GET "https://api.alcf.anl.gov/api/v1/account/projects/${project_id}/project_allocations" \
     -H "Authorization: Bearer ${access_token}"
```
////

//// tab | Python

```python
import requests
from alcf_tokens.auth import get_access_token

# Create headers with access token
access_token = get_access_token("iri")
headers = {
    "Authorization": f"Bearer {access_token}",
    "Content-Type": "application/json"
}

project_id = "<project_id>"

response = requests.get(
    f"https://api.alcf.anl.gov/api/v1/account/projects/{project_id}/project_allocations",
    headers=headers
)

print(response.status_code)
print(response.json())
```
////
///

/// details | 5.4. Get a Specific Project Allocation

Returns a specific project allocation by its ID.

//// tab | cURL

```bash
#!/bin/bash
access_token=$(alcf-tokens get-token iri)

project_id="<project_id>"
project_allocation_id="<allocation_id>"

curl -X GET "https://api.alcf.anl.gov/api/v1/account/projects/${project_id}/project_allocations/${project_allocation_id}" \
     -H "Authorization: Bearer ${access_token}"
```
////

//// tab | Python

```python
import requests
from alcf_tokens.auth import get_access_token

# Create headers with access token
access_token = get_access_token("iri")
headers = {
    "Authorization": f"Bearer {access_token}",
    "Content-Type": "application/json"
}

project_id = "<project_id>"
project_allocation_id = "<allocation_id>"

response = requests.get(
    f"https://api.alcf.anl.gov/api/v1/account/projects/{project_id}/project_allocations/{project_allocation_id}",
    headers=headers
)

print(response.status_code)
print(response.json())
```
////
///


## Troubleshooting

- **Permission Denied:** Your token may have expired or you may not be authenticated with your ALCF credentials. Logout from Globus at [app.globus.org/logout](https://app.globus.org/logout), clear your browser cache or use an incognito window, and re-authenticate.
- **IdentityMismatchError: Detected a change in identity:** This happens when trying to get an access token using a Globus identity that is not linked to the one you previously used. Locate your tokens file (typically at `~/.globus/app/7f3e61f5-e0de-4e8f-9150-0a62c65dda63/alcf_tokens/tokens.json` or `~/.globus/app/8b84fc2d-49e9-49ea-b54d-b3a29a70cf31/alcf_facility_api_app/tokens.json`), delete it, and restart the authentication process.

## Further Information
Further information on and examples for the IRI API may be found at [https://github.com/doe-iri](https://github.com/doe-iri)

## Contact Us

For questions or support, please contact [ALCF Support](mailto:support@alcf.anl.gov?subject=Facility%20API).
