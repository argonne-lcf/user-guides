# Miscellaneous

## Porting applications to the CS-2

Cerebras documentation for porting code to run on a Cerebras CS-2 system:<br>
[Ways to port your model](https://docs.cerebras.net/en/latest/wsc/port/index.html)

## Grafana WsJob Dashboard for Cerebras jobs
A Grafana dashboard provides support for visualizing, querying, and exploring the CS2 system’s metrics and enables to access system logs and traces.
See the Cerebras documentation for the [Job Information Dashboard](https://docs.cerebras.net/en/latest/wsc/getting-started/grafana.html#wsjob-dashboard)

Here is a summary (tested to work on Ubuntu and MacOS)<br>

On your work machine with a web browser, e.g. your laptop,<br>
edit /etc/hosts, using your editor of choice
```console
sudo nano /etc/hosts
```
Add this line
```console
127.0.0.1	grafana.cerebras1.lab.alcf.anl.gov
```
Save, and exit the editor

Download the Grafana certificate present on the Cerebras node at /opt/cerebras/certs/grafana_tls.crt to your local machine. To add this certificate to your browser keychain, 

1. On chrome, go to Settings->Privacy and security->Security->Manage device certificates
2. Select System under "System Keychains" on the left hand side of your screen. Also select the "Certificate" tab.
3. Drag and drop the downloaded certificate. Once it is added, it is visible as "lab.alcf.anl.gov"
   ![Cerebras Wafer-Scale Cluster connection diagram](files/grafana_ctl.png)
4. Select the certificate, and ensure that the "Trust" section is set to "Always Trust"
   ![Cerebras Wafer-Scale Cluster connection diagram](files/Trust_ctl.png)


On your work machine with a web browser, e.g. your laptop,<br>
tunnel the grafana https port on the cerebras grafana host through to localhost
```
ssh -L 8443:grafana.cerebras1.lab.alcf.anl.gov:443 arnoldw@cer-login-03.ai.alcf.anl.gov
```

Point a browser at grafana. (Tested with Firefox and Chrome/Brave)<br>
Open browser to a job grafana url shown in csctl get jobs, adding :8443 to hostname, e.g.<br>
```console
https://grafana.cerebras1.lab.alcf.anl.gov:8443/d/WebHNShVz/wsjob-dashboard?orgId=1&var-wsjob=wsjob-49b7uuojdelvtrcxu3cwbw&from=1684859330000&to=noww
```

Login to the dashboard with user admin, and password prom-operator


<!---
## Determining the CS-2 version

TODO
Need another approach for the new worker nodes with general ANL access.
These queries will only work on cer-usr-01 due to networking constraints:
```
...$ # Query the firmware level for cs2-01
...$ curl -k -X GET 'https://10.140.89.251/redfish/v1/Managers/manager' --header 'Authorization: Basic YWRtaW46YWRtaW4=' 2> /dev/null | python -m json.tool | grep FirmwareVersion
 "FirmwareVersion": "1.7.1-202302011928-7-9d6aea6f",
...$

...$ # Query the firmware level for cs2-02
...$ curl -k -X GET 'https://10.140.89.252/redfish/v1/Managers/manager' --header 'Authorization: Basic YWRtaW46YWRtaW4=' 2> /dev/null | python -m json.tool | grep FirmwareVersion
 "FirmwareVersion": "1.7.1-202302011928-7-9d6aea6f",
...$

```
--->
<!--- NO LONGER NEEDED; direct login.
## Copying files
To copy a file to your CS-2 home dir (same on both CS-2 clusters), replacing <strong>both instances</strong> of ALCFUserID with your ALCF user id:
```console
scp -o "ProxyJump ALCFUserID@cerebras.alcf.anl.gov" filename ALCFUserID@cs2-01-master:~/
```

To copy a file from your CS-2 home dir (same on both CS-2 clusters) to the current local directory, replacing <strong>both instances</strong> of ALCFUserID with your ALCF user id:
```console
scp -o "ProxyJump ALCFUserID@cerebras.alcf.anl.gov" ALCFUserID@cs2-01-master:~/filename .
```
--->

<!---
NO LONGER NEEDED - python environments are available, and singularity not available

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
`unzip` is available on the CS-2 worker nodes.

Note: the kaggle download shown above included two identical copies of the dataset; one copy was in a subdirectory.
--->
