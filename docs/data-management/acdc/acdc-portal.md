# ACDC Globus Portal

## Share metadata using [ACDC Globus Portal](https://acdc.alcf.anl.gov/)

To share data using ACDC portal. A user would need to follow the following steps

1. Move data to Eagle File System. If you are new to the ALCF, follow these instructions on [transferring data to eagle](transferring-data-to-eagle.md)
If you already have an ALCF account, follow these instructions on how to share your data:
[Sharing Data to Eagle](eagle-data-sharing.md)

2. Create a Guest Collection (Only PI) to enable [data sharing](eagle-data-sharing.md#Creating a Guest Collection)

3. Now you will have to generate metadata. The metadata needs to comply with [Globus Search API](https://docs.globus.org/api/search/). Download this [helper script](https://github.com/argonne-lcf/data-curator/blob/main/globus-data-curator.py) to create metadata for Globus. The helper script will parse through your entire project space and generate metadata for you from the login nodes. Feel free to modify the script as needed to collect the metadata you want to publish. This script will generate a metadata.json file
```bash
wget -cO - https://raw.githubusercontent.com/argonne-lcf/data-curator/main/globus-data-curator.py?token=GHSAT0AAAAAAB7QNCRHTLBFJ74LIZQSE6S4ZA5L5OA > globus-data-curator.py 
python3 globus-data-curator.py -p <path_to_project> -n <Name of your project>
```
4. Once you have the metadata, you will need to create a "Globus Index". This can be done using [Globus Cli](https://docs.globus.org/cli/) on the login node.
```bash
module load conda
python3 -m venv ~/envs/globus-env #do this once
source ~/envs/globus-env/bin/activate
pip install globus-cli #do this once
globus search index create <name of project> <description>
```
5. The UUID for the index generated in the previous step in important. It can then be used to ingest the metadata
```bash
globus search ingest <index-UUID> metadata.json
```

6. Finally, contact support@alcf.anl.gov with the following information
   ```bash
    Please add the following index to the Globus Portal
    Project Name:
    Project Description: 
    Index UUID:
   ```
   The UUID will be added to the portal for others to view
