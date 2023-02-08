# Miscellaneous

## Porting applications to the CS-2

Cerebras’s mature Python support is built around [Cerebras Estimator](https://docs.cerebras.net/en/latest/tensorflow-docs/porting-tf-to-cs/example-walk-through-cs-estimator.html?highlight=estimator), which inherits from [TensorFlow Estimator](https://www.tensorflow.org/api_docs/python/tf/estimator/Estimator).<br>
A Keras model can be converted to TF Estimator, and hence to a Cerebras Estimator. See [https://www.tensorflow.org/tutorials/estimator/keras_model_to_estimator](https://www.tensorflow.org/tutorials/estimator/keras_model_to_estimator) for more information on conversion of Keras models.<br>

Cerebras has recently introduced PyTorch support. The PyTorch support is documented at [Cerebras Software Documentation](https://docs.cerebras.net/en/latest/) in the section DEVELOP WITH PYTORCH. 

Cerebras has guides for porting TensorFlow and PyTorch models:<br>
[Port TensorFlow to Cerebras](https://docs.cerebras.net/en/latest/tensorflow-docs/porting-tf-to-cs/index.html)</br>
[Porting PyTorch Model to CS](https://docs.cerebras.net/en/latest/pytorch-docs/adapting-pytorch-to-cs.html)<br>
This is Cerebras's list of the TensorFlow layers that they support (for the current version): [Supported TensorFlow Layers](https://docs.cerebras.net/en/latest/tensorflow-docs/tf-layers/index.html)<br>
This is Cerebras's list of the PyTorch operations supported (for the current version): [Supported PyTorch Ops](https://docs.cerebras.net/en/latest/pytorch-docs/pytorch-ops.html)


When porting, it is often helpful to study a related example in the Cerebras modelzoo.<br>
A copy of the modelzoo for the install release is at ```/software/cerebras/model_zoo/modelzoo/```<br>
Both the `README.md` files and source code in the modelzoo can be quite helpful.

## Determining the CS-2 version

<!---
[TODO should this API/auth string be made public? Alternative supplied that inspects the singularity container.]
Note: replace the IP address with the CS_IP for the CS-2 cluster being used.<br>
[TODO could use CS_IP environment variable if set.]<br>
[balin: I was not able to make the `curl` command below run, even removing the `<br>` characters. Bill Arnold - checked with cerebras and this isn't working with the cs2-02 (does on cs2-01) that we're going live with, so commenting out.<br>
--->
<!---
```console
...$ # Query the software level in the singularity image
...$ singularity sif dump 1 /software/cerebras/cs2-02/container/cbcore_latest.sif | grep "from"
from: cbcore:1.1.1-202203171919-5-6e2dbf07
...# or singularity sif dump 1 /software/cerebras/cs2-01/container/cbcore_latest.sif | grep "from"
...$
```
--->

These queries will only work on cs2-01 due to networking constraints:
```
...$ # Query the firmware level for cs2-01
...$ curl -k -X GET 'https://192.168.120.30/redfish/v1/Managers/manager' --header 'Authorization: Basic YWRtaW46YWRtaW4=' 2> /dev/null  | python -m json.tool | grep FirmwareVersion
"FirmwareVersion": "1.1.1-202203171919-5-879ff4ef",
...$

...$ # Query the firmware level for cs2-02 (from cs2-01)
...$ curl -k -X GET 'https://192.168.120.50/redfish/v1/Managers/manager' --header 'Authorization: Basic YWRtaW46YWRtaW4=' 2> /dev/null  | python -m json.tool | grep FirmwareVersion
"FirmwareVersion": "1.1.1-202203171919-5-879ff4ef",
...$

```
<!---
## Viewing the Cerebras V 1.3 documentation
The Cerebras V 1.3 documentation is stored on the Cerebras systems and can be served to be viewed with a local browser by running the following in a command prompt on your workstation/laptop.<br>
*Change the <strong>ALCFUserID</strong> to your id.*<br>
If there is a port conflict, use a different port number in either the second and third port number instance, or all three of them.
```console
ssh -t -L localhost:8089:localhost:8089 ALCFUserID@cerebras.alcf.anl.gov  "cd /software/cerebras/docs/V1.3/;python3 -m http.server 8089"
```
To view the documentation, copy the url into your browser as indicated in the output of the above command (e.g., `http://0.0.0.0:8089/`).

This v 1.3 documentation tree can also be copied to your laptop/workstation and the files can be viewed locally with a browser. The Cerebras system has a tar file at /software/cerebras/docs/Cerebras_ML_SW_Docs_V1.3.tar
--->

## Copying files
To copy a file to your CS-2 home dir (same on both CS2 clusters), replacing <strong>both instances</strong> of ALCFUserID with your ALCF user id:
```console
scp -o "ProxyJump ALCFUserID@cerebras.alcf.anl.gov" filename ALCFUserID@cs2-01-master:~/
```

To copy a file from your CS-2 home dir (same on both CS2 clusters) to the current local directory, replacing <strong>both instances</strong> of ALCFUserID with your ALCF user id:
```console
scp -o "ProxyJump ALCFUserID@cerebras.alcf.anl.gov" ALCFUserID@cs2-01-master:~/filename .
```

## Downloading a Kaggle competition dataset to a CS-2 node using the command line

These notes may be helpful for downloading some Kaggle datasets

Inside a singularity shell (e.g. `singularity shell -B /opt:/opt /software/cerebras/cs2-02/container/cbcore_latest.sif` )

```console
virtualenv env
source env/bin/activate
pip3 install kaggle
```

Go to www.kaggle.com in a browser, log in (create account if first time). In user(icon upper right) -&gt; Account tab, there is a button (scroll down) to "Create New API Token". Click it. It will open a download window for a one line json. 

put the json in `~/.kaggle/kaggle.json`</br>
e.g. scp the downloaded file, or single quote the json text and echo it as shown</br>
```console
mkdir ~/.kaggle
echo '{"username":"REDACTED","key":"REDACTED"}' > ~/.kaggle/kaggle.json
chmod 600 ~/.kaggle/kaggle.json
```

On www.kaggle.com, the kaggle api command for download of a dataset is displayed in the data tab. It can be selected and copied to the local clipboard, or copied with the "Copy API command to clipboard" icon.<br>
Before attempting a download, if there is a button on the kaggle download page to agree to any terms and conditions, e.g. agreement to the competition rules, click on it (after reading them); downloads with your access token will fail with a 403 error until you agree to those T&Cs.

Paste the API command to the command line inside the singularity shell with the venv activated. E.g.<br>
```bash
kaggle datasets download -d mhskjelvareid/dagm-2007-competition-dataset-optical-inspection
```

It will download as a zip file. 

Exit the singularity container (with `exit`), then unzip the dataset zip file.<br>
`unzip` is available on the CS2 worker nodes.

Note: the kaggle download shown above included two identical copies of the dataset; one copy was in a subdirectory.




