# PostgreSQL and SQLite

## Theta
The module `postgresql/9.6.12` is available on Theta.  Use `module load postgresql` to add the Postgres 9.6 binaries to your search `PATH`.

## Cooley 
On Cooley, the Postgres 9.6 binaries can be added to your search `PATH` via: ```source /soft/datascience/balsam/postgres-envs.sh```

## Downloading binaries
To install your own Postgres binaries visit [enterpriseDB.com](https://www.enterprisedb.com/download-postgresql-binaries).

## Creating a New Database Cluster
To create a new database cluster, use: ```initdb -D <Database Path> -U $USER```

Inside the database directory, you will need to edit the file `postgresql.conf` as follows. Set the options below:
```
listen_addresses = '*'
port = 12345 # choose a random high-numbered port that isn't yet bound on the host
```

To allow connections on all interfaces without requiring authentication, you will need to edit the Host-Based Authentication rules as follows.  Edit the file `pg_hba.conf` by appending the following line:
```
host all all 0.0.0.0/0 trust
```

The above instructions assume you are operating in a secure environment and are willing to blindly accept connections from any host in the network. This a convenient but insecure option.

If you want to secure connections to your database, refer to the documentation on [host-based authentication](https://www.postgresql.org/docs/9.6/auth-pg-hba-conf.html) and consider using `md5` rather than `trust` based authentication.

## Starting and Stopping the Database Cluster
You can start the database located at path `./foo` with with:
```
pg_ctl -D ./foo start
```

Stop the database with:
```
pg_ctl -D ./foo stop
```
or send a `SIGTERM` to the leader `postgres` process, which will shut down gracefully.

## Connecting to a database
Assuming the database is running on port `12345` of host `thetalogin1`, you can start a psql session in the terminal as follows:
```
psql -p 12345 -h thetalogin1 -d postgres
```
The `-h` argument defaults to localhost as is therefore not necessary if you are connecting from the same machine that's currently hosting the database cluster.

If using a Python ORM like [Django](https://docs.djangoproject.com/en/2.2/ref/settings/#databases), you will need to configure the `DATABASES` option in your `settings.py` to point at the same host and port, according to the documentation.

**Note:** that we connected to the default superuser database called `postgres`. You will want to create a new database under your cluster as follows:
```
createdb <Database Name> -h <Host> -p <Port>
```

For example, let's create a database called ToDoList and then connect to it:
```
createdb ToDoList -h thetalogin1 -p 12345
psql -p 12345 -h thetalogin1 -d ToDoList
```

### Creating tables

To get started with SQL follow the [Postgres tutorial](https://www.postgresql.org/docs/9.6/tutorial-sql-intro.html).

A minimal example looks like:
```
CREATE TABLE todo_list (
   task varchar(200),
   date date,
   priority int
);
 
INSERT INTO todo_list VALUES ('Learn SQL', '2019-10-30', 4);
 
SELECT * FROM todo_list;
```

## SQLite
```
import sqlite3
conn = sqlite3.connect('example.db')
c = conn.cursor()
```

