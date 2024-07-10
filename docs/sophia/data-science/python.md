# Setting Up a Python Virtual Environment on Sophia

## Default Python Version
The default Python on Sophia is located at `/usr/bin/python` with version 3.9.18.

## Creating a Virtual Environment
Creating a virtual environment allows you to manage dependencies for your Python projects independently. 
This is particularly useful when working on multiple projects with different dependencies or versions of packages. 
Follow these steps to set up a virtual environment on Sophia:

1. **Create a Virtual Environment**:
    ```bash
    python -m venv myenv
    ```
    Replace `myenv` with your preferred name for the virtual environment. 
    This will create a directory with the specified name containing the virtual environment.

2. **Activate the Virtual Environment**:
    ```bash
    source myenv/bin/activate
    ```
    After activation, your command prompt will change to indicate the virtual environment is active.

3. **Upgrade `pip`**:
    
    While this step is optional, it is a good habit to use the latest version of `pip`:
    ```bash
    pip install --upgrade pip
    ```

4. **Install Packages**:
    Use `pip` to install necessary packages:
    ```bash
    pip install package_name
    ```
    Replace `package_name` with the name of the package you want to install.

## Creating a Jupyter Kernel

If you would like to use your Python virtual environment on JupyterHub, you will need to create a Jupyter kernel for it.

1. **Install `ipykernel`**:
    Ensure `ipykernel` is installed in your virtual environment:
    ```bash
    pip install ipykernel
    ```

2. **Create a Jupyter Kernel**:
    ```bash
    python -m ipykernel install --user --name=myenv --display-name "Jupyter (myenv)"
    ```
    Replace `myenv` with the name of your virtual environment and `"Jupyter (myenv)"` with the display name you want for the kernel in JupyterHub.