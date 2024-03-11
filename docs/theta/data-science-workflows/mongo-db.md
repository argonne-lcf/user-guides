---
search:
  exclude: true
---

# MongoDB

MongoDB is a NoSQL database which stores information as binary JSON's (BSONS). All of the details mentioned in this document has been tested against MongoDB version 3.4. Kindly consult the official online manual in case you would like to port another version of the MongoDB.In this section, we will see how to setup MongoDB on ALCF machines (i.e. Cooley and Theta). The central aspect of running MongoDB involves the definition of the server and the client. In the following sections, we will see how to:
- Launch a Mongo server instance on the login node.
- Establish the connection between the client and server.

## MongoDB at ALCF Cooley

As a first step, we must define a configuration ('.conf') file. An example script is shown below.
```
processManagement:
fork: true
storage:
dbPath: /path/mongodb/db
systemLog:
destination: file
path: "/path/log/mongodb.log"
logAppend: true
net:
bindIp: 172.23.100.210
port:27017  #--> Has to be modified by the user!
```

Please save the script to mongodb.cooley.conf. Adapt the dbpath and path to your local path. The port number of 27017 is the default for MongoDB. Each server instance of MongoDB should have a unique port number and this should be changed to a sensible number (e.g.54392, 2459 etc.). Hence it is highly recommended to set port number by a random number generator in order to avoid conflict with other users.

**Note:** Make sure the bindIpis set correctly by running the command ‘ifconfig -v’  and see if it matches with the ib0 IP.

```
$/path/bin/mongod -f /path/mongodb.cooley.conf;#Launches the server
$/path/bin/mongod -f /path/mongodb.cooley.conf --shutdown #To shutdown the server
```

Once the server is launched, the user can connect to it. The default way to connect to the database server would be the Mongo shell as shown below.

$/path/bin/mongo --host 172.23.100.210  --port 27017 # Must match with the  bindIP and port in the conf file!

The connection request to the database server can come any number of methods such as a Jupyter notebook, or a standalone Python script or it can even come from a job running on the worker nodes. A sample Python script to connect to the server, that utilizes PyMongo driver version 3.4.0 is shown below. For more details, please refer to the official documentations of MongoDB.

```
from pymongo import MongoClient
import pymongo
print(pymongo.__version__) # To make sure your MongoDB version is consistent with the driver!
client = MongoClient('mongodb://172.23.100.210:27017')  # Must match with the  bindIP and port in the conf file!
db = client.Myproject       # Creates a new database with the name 'Myproject' 
db.MyCollection.create_index([('Myindex', pymongo.ASCENDING)], unique=True)  # Creates a new collection with a unique index. 
sorted(list(db.MyCollection.index_information()))
```

## MongoDB at ALCF Theta

All the steps explained in the previous step are applicable to Theta as well, expect the configuration file.

processManagement:
```
fork: true
storage:
dbPath: /path/mongodb/db
systemLog:
destination: file
path: "/path/log/mongodb.log"
logAppend: true
net:
bindIp: 10.236.1.194
port: 27017  #--> Has to be modified by the user!
```

Please save the script to mongodb.theta.conf. Adapt the dbpath and path to your local path. The port number of 27017 is the default for MongoDB. Each server instance of MongoDB should have a unique port number, and this should be changed to a sensible number (e.g.54392, 2459 etc.). Hence it is highly recommended to set port number by a random number generator in order to avoid conflict with other users. 

**Note:** Make sure the bindIp is set correctly by running the command ip a and see if it matches with the bindIP in the conf file
```
$/path/bin/mongod -f /path/mongodb.theta.conf   #Launches the server

$/path/bin/mongod -f /path/mongodb.theta.conf --shutdown  #To shutdown the server
```

Once the server is launched, the user can connect to it. Similar to the previous section, the connection to mongo shell can be achieved as shown below: $/path/bin/mongo --host 10.236.1.194 --port 27017 # Must match with the bindIP and port in the conf file!

For more information please look at [https://docs.mongodb.com/v3.4/introduction](https://docs.mongodb.com/v3.4/introduction)
