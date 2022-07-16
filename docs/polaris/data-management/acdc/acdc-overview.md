# ALCF Community Data Co-Op (ACDC) 

## Overview of the ALCF Community Data Co-Op (ACDC)

The ALCF Community Data Co-Op (ACDC) powers data-driven research by providing a platform for data access and sharing, and value-added services for data discovery and analysis.

A fundamental aspect of ACDC is a data fabric that allows programmatic data access, and straightforward large-scale data sharing with collaborators via [Globus services](www.globus.org).This provides a platform to build out different modalities for data access and use, such as indexing of data for discovery, data portals for interactive search and access, and accessible analysis services. ACDC will continue to be expanded to deliver ALCF users the platform to build customizable and accessible services towards the goal of supporting data-driven discoveries.

## Data access and sharing 
ALCF project PIs can share data on Eagle with their collaborators, making facility accounts unnecessary. With this service, the friction of data sharing amongst collaborators is eliminated – there is no need to create copies of data for sharing, or allocation and accounts just to access data. ALCF PIs can grant access to data, at read-only or read/write access levels. Non-ALCF users throughout the scientific community, who have been granted permissions, can access the data on Eagle filesystem using Globus.

Access to the data for ALCF users and collaborators is supported via bulk transfer (Globus transfer) or direct browser-based access (via HTTP/S). Direct connections to high-speed external networks permit data access at many gigabytes per second. Management of permissions and access is via a web application or command line clients, or directly via an Application Programming Interface (APIs). The interactivity permitted by the APIs distinguishes ACDC from the ALCF’s previous storage systems and presents users with many possibilities for data control and distribution.

## Data portal for discovery and access 
ACDC’s fully supported production environment is the next step in the expansion of edge services that blur the boundaries between experimental laboratories and computing facilities. The use and prominence of such services at the ALCF are only expected to increase as they become more integral to the facility’s ability to deliver data-driven scientific discoveries.

ACDC includes several project-specific data portals that enable search and discovery of the data hosted on Eagle. The portals allow users to craft queries and filters to find specific sets of data that match their criteria and use faceted search for the discovery of data. Portals also provide the framework for other interfaces including data processing capabilities, all secured with authentication and configured authorization policy.

The ACDC portal is a deployment of Django Globus Portal Framework customized for a variety of different projects For most of these projects, the search metadata links directly to data on Eagle, with browser-based download, preview, and rendering of files, and bulk data access.

## Getting Started
1. **Request an allocation:** Researchers or PIs request an allocation on Eagle, and a project allocation is created upon request acceptance.
2. **Manage Access:** PIs can manage the space independently or assign other users to manage the space, as well as provide other users with read or read/write access for folders in the space. Globus groups and identities are used to manage such access.
3. **Authentication:** Globus is used for authentication and identity needed to access the system. As Globus has built-in support for federated logins, users can access ACDC using their campus or institution federated username and passcode

If you are new to the ALCF, follow these instructions on how to transfer your data to ACDC:
== Add page: Transferring your data to ACDC ==

If you already have an ALCF account, follow these instructions on how to share your data:
[Sharing Data to Eagle](eagle-data-sharing.md)


