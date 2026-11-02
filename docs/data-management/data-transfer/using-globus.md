---
tags:
  - Globus
keywords:
  - transfer
  - endpoint
  - collection
  - DTN
---

# Using Globus

[Globus](http://www.globus.org/) addresses the challenges faced by researchers in moving, sharing, and archiving large volumes of data among distributed sites. With Globus, you hand off data movement tasks to a hosted service that manages the entire operation. It monitors performance and errors, retries failed transfers, corrects problems automatically whenever possible, and reports status to keep you informed and focused on your research.

Command line and web-based interfaces are available. The command line interface [Globus CLI](https://docs.globus.org/cli/18729361) is the method of choice for script-based workflows on your local machine. Globus also provides a [REST-style transfer API](https://docs.globus.org/api/transfer/) for advanced use cases that require scripting and automation.

## Getting Started

Basic documentation for getting started with Globus can be found at the following URL:
[https://docs.globus.org/how-to/](https://docs.globus.org/how-to/)

## ALCF Globus Collections

Several Globus Collections for `/home`, Eagle, Grand, and HPSS are available to ALCF users, allowing users to perform wide and local area data transfers. Access is provided via the following Globus endpoints. Access the endpoints associated to your storage allocation using the collection names listed below.

- [Home](https://app.globus.org/file-manager?origin_id=9032dd3a-e841-4687-a163-2720da731b5b&two_pane=false), which is where your home directory resides for Polaris, Sophia, and Crux systems: `alcf#dtn_home` for accessing `/home` (i.e., home directories on the agile-home filesystem). Use the path `/<ALCF username>`
- [HPSS](https://app.globus.org/file-manager?origin_id=bed0e34e-c7fa-4e8c-b291-8f70e634371e&two_pane=false): `alcf#dtn_hpss`
- [Eagle filesystem](https://app.globus.org/file-manager?origin_id=05d2c76a-e867-4f67-aa57-76edeb0beda0&two_pane=false): `alcf#dtn_eagle` for accessing `/lus/eagle/projects` or `/eagle` (i.e., project directories on the Eagle filesystem). Use the path `/eagle/<project>`
- [Grand filesystem](https://app.globus.org/file-manager?origin_id=3caddd4a-bb35-4c3d-9101-d9a0ad7f3a30&two_pane=false): `alcf#dtn_grand` for accessing `/lus/grand/projects` or `/grand` (i.e., project directories on the Grand filesystem). Use the path `/grand/<project>`
- [Flare filesystem](https://app.globus.org/file-manager?origin_id=f39a7a0f-5bfc-46ce-9615-ba9f8592814f&two_pane=false): `alcf#dtn_flare` for accessing `/lus/flare/projects` or `/flare` (i.e., project directories on the Flare filesystem) on Aurora. Use the path `/<project>`

After [registering](https://app.globus.org/), simply use the appropriate ALCF collection, as well as other sources or destinations. Use your ALCF credentials (your one-time passcode from the MobilePASS+ app, or PIN + passcode from your physical hardware token) to activate the ALCF collection.

[Globus Connect Personal](https://www.globus.org/globus-connect-personal) allows users to add laptops or desktops as an endpoint to Globus in just a few steps. After you set up Globus Connect Personal, Globus can be used to transfer files to and from your computer.

Visit [moving data to Aurora](../../aurora/data-management/moving_data_to_aurora/globus.md) for more information on data ingress and egress.

## References

[Research Data Management with Globus (2019)](https://www.alcf.anl.gov/support-center/training-assets/research-data-management-globus)  
<iframe width="560" height="315" src="https://www.youtube.com/embed/1nCfWslDrf8" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
