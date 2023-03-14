# Virtual Environments to Customize Environment

## Using a Virtual Venv

To create a virtual environment, one can use the --system-site-packages flag:

```bash
python -m venv --system-site-packages my_env
source my_env/bin/activate
```

### System Site Packages

There are many packages available on the system.
Run the following Python script to retrieve the
location of the packages:

```python
import sys
site_packages = next(p for p in sys.path if 'site-packages' in p)
print(site_packages)
```

Given the location of the packages, one may list the packages.
For example:

```bash
ls -al /opt/sambaflow/venv/lib/python3.7/site-packages
```

## Installing Packages

Install packages in the normal manner such as:

```bash
python3 -m pip install "SomeProject"
```

For more details see [Use pip for installing](https://packaging.python.org/en/latest/tutorials/installing-packages/#use-pip-for-installing).

To install a different version of a package that is already installed in one's environment, one can use:

```bash
pip install --ignore-installed  ... # or -I
```

Note: Conda is not supported on the SambaNova system.
