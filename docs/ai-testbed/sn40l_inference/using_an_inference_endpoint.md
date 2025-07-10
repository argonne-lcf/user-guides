# Using an inference endpoint

## getting api information files from your home directory
The API information files are of form [TODO]. The filename contains [todo]. The file contains [todo]

## Generic Code examples 
### Using environment variables for endpoint url, api key, model name
The information files may be sourced to put the necessary values into environment variables. For example:
```console
source [sample filename]
```
This would set the environment variables API_URL, API_KEY, API_MODEL_NAME
(or openai equivalents)
### Python example
Make a virtual env and activate it, or use an existing virtualenv or conda env.
```console
virtualenv create openai_venv
source openai_venv/bin/activate
```
Then install the openai package needed for chat completions
```console
pip install openai
```
Write a python script that

- imports the openai package
- makes an openai client
- calls the chat.completions.create method
- extracts what is wanted from the response

Simple sample python script, that uses environment variables `OPENAI_API_KEY`, `OPENAI_BASE_URL`, and `MODEL_NAME`, and accepts a (quoted) prompt as a command line parameter:
```python
import os
import openai
import sys

client = openai.OpenAI(
    api_key=os.environ.get("OPENAI_API_KEY"),
    base_url=os.environ.get("OPENAI_BASE_URL")
)

modelname = os.environ.get("MODEL_NAME")
query = sys.argv[1]
response = client.chat.completions.create(
    model=modelname,
    messages=[{"role":"user","content":query}],
    temperature =  0.1,
    top_p = 0.1
)

print(response.choices[0].message.content)
```

### curl example
Sample curl command.

```bash
export PROMPT="What are \"telescoping generations\" in biology?"
curl -H "Authorization: Bearer ${OPENAI_API_KEY}" \
     -H "Content-Type: application/json" \
     -d '{
	"stream": true,
	"model": "'${MODEL_NAME}'",
	"messages": [
		{
			"role": "user",
			"content": "'${PROMPT}'"
		}
	]
	}' \
     -X POST ${OPENAI_BASE_URL}/chat/completions

```

