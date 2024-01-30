# GroqView profiler and visualizer tool

This section covers how to remotely use the GroqView profiler and visualizer tool.

## GroqView sample

Groq compiles produce an accurate and detailed model of the performance of a model's execution on groq cards. There is no need to run a model on groqcards to use GroqView.<br>
The GroqView example adds the "groqview=True" parameter to the `groqit` call, then calls the `groqview()` method on the model returned by `groqit`.<br>
This is the relevant code when using GroqFlow. It tries to retrieve the compiled model from the cache, compiles the model on a cache miss, then calls `groqview()`.<br>
From `groqflow/examples/pytorch/groqview.py`: <br>
```{python}
# Build model
gmodel = groqit(pytorch_model, inputs, groqview=True)
# Open GroqView
gmodel.groqview()
```

## Run the sample
On a groq node, run the groqview.py sample (or any script that includes similar code). Note the port number chosen by GroqView.<br>
```console
conda activate groqflow
cd ~/groqflow/examples/pytorch
python groqview.py
# You will see something like the following.
# The port number may be different.
...
Open your web browser:
    http://localhost:8439
```

## Forward the port to your machine with a browser

On your laptop/user machine with a display, set up a 2-hop ssh tunnel.<br>
Set `$GN_HOSTNAME` to the name of the host where job is running<br>
```console
export GN_HOSTNAME=groq-r01-gn-09
# Modify the port number if GroqView has chosen a different port.
# This might happen if another user is also using GroqView.
# Also, another user may be using the port on the login host.
# `groq-login-01.ai.alcf.anl.gov` can be used as well.
ssh -L 8439:localhost:8439 arnoldw@groq-login-02.ai.alcf.anl.gov -t ssh -L 8439:localhost:8439 -N $GN_HOSTNAME
# When complete, "ctrl-c" or equivalent in the console where the ssh tunnel
# was started will terminate both parts of a ssh tunnel set up this way.
```

## Access the GroqView server for your application:

Point a Google Chrome-family web browser at this url, adjusting the port number if necessary.
(Chrome, Brave, Vivaldi, Opera tested.)
```console
http://localhost:8439
```
