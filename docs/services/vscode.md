# Using Visual Studio Code with Remote SSH and C++ (Linux Remote)

## Overview

This document explains how to install Visual Studio Code (VS Code) on your local machine, add the Remote - SSH extension, configure SSH for Windows, macOS, and Linux, and compile a simple `helloworld.cpp` program on a **remote ALCF server**. The compilation and execution happen on the remote Linux system; your local machine is only used to run [VS Code and initiate the SSH connection](https://code.visualstudio.com/docs/remote/ssh).

## Installing Visual Studio Code (local machine)

[VS Code](https://code.visualstudio.com) is a free, cross‑platform code editor available for Windows, macOS, and Linux.

### Download

1. Open a web browser and go to the official download page: [https://code.visualstudio.com](https://code.visualstudio.com).
2. Choose the installer for your operating system (Windows, macOS, or Linux).

### Install on Windows

1. Run the downloaded installer (`VSCodeUserSetup-{version}.exe`).
2. Accept the license agreement and keep the default install location unless you have a specific requirement.
3. In the setup options, it is recommended to enable:
   - “Add to PATH” so you can run `code .` from a terminal.
   - “Register Code as an editor for supported file types”.
4. Click **Install**, then **Finish** to launch VS Code.

By default, VS Code is installed under:

```text
C:\Users\<Username>\AppData\Local\Programs\Microsoft VS Code
```

on [Windows](https://code.visualstudio.com/docs/setup/windows).

### Install on macOS and Linux

1. From the same download page, choose the installer or package for your platform (`.dmg` for macOS, `.deb`/`.rpm` or other package for Linux).
2. On Linux, install the package using your distribution’s package manager (for example, `sudo apt install ./code_*.deb` on Debian/Ubuntu).
3. Alternatively, some distributions provide VS Code via `snap` or `flatpak`; follow your distribution’s recommended instructions.

After installation, you can start VS Code from your applications menu or from a terminal with the `code` command (if added to your PATH).

## Installing the Remote - SSH extension

The Remote - SSH extension lets you use any remote machine with an SSH server as your development environment from inside VS Code.

### Install the extension

1. Open VS Code on your local machine.
2. Click the **Extensions** view icon on the Activity Bar (or press `Ctrl+Shift+X`).
3. In the search box, type `remote ssh`.
4. Install **Remote - SSH** (publisher: Microsoft).

After installation, a green remote status indicator appears in the bottom‑left corner of the VS Code window showing whether VS Code is running locally or connected to a remote host.

## SSH client and host requirements

To use Remote - SSH, your **local** machine needs an OpenSSH‑compatible SSH client, and the **remote** machine must be a Linux system running an SSH server.

- Supported local OS: Windows, macOS, Linux.
- Remote host: Linux with SSH server enabled (typical HPC or campus compute cluster).
- At least 1 GB of RAM is required on the remote host; 2 GB and a 2‑core CPU are recommended.

Most Linux and macOS systems already have the `ssh` client installed; on Windows 10+ you can enable the “OpenSSH Client” via **Settings → Optional Features**.

## Configuring SSH (local side)

VS Code Remote - SSH can work directly with an SSH command (`user@host`), but for repeatable connections it is best to configure an SSH config file.

### SSH config file locations

- **Linux/macOS**: `~/.ssh/config` (note that this file should have permssions for reading `chmod 644 ~/.ssh/config`)
- **Windows (OpenSSH client)**: `C:\Users\<username>\.ssh\config`

VS Code can help you edit or create this config file when you use **Remote-SSH: Add New SSH Host...**.

### Example SSH config entry

Example entry for a `Polaris` and `Aurora` :

```sshconfig
Host *
    ControlMaster auto
    ControlPath ~/.ssh/master-%r@%h:%p
    ControlPersist 10m
    ForwardX11 yes
    LogLevel QUIET
    ServerAliveInterval 60
    ServerAliveCountMax 3
    ConnectTimeout 0

Host polaris
    HostName polaris.alcf.anl.gov
    User your_username
    Port 22
    ForwardAgent yes

Host aurora
    HostName aurora.alcf.anl.gov
    User your_username
    ForwardAgent yes

```

Explanation:

- `Host` is the short name you will use in VS Code (for example, `myserver`).
- `HostName` is the full DNS name or IP address of the remote Linux server.
- `User` is your account name on the remote server.
- `Port` is the SSH port (22 is standard; adjust if your site uses a different port).

## Adding a new SSH host from VS Code

VS Code can guide you through creating or updating the SSH configuration file.

1. Open the **Command Palette** (`Ctrl+Shift+P`).
2. Run **Remote-SSH: Add New SSH Host...**.
3. When prompted, enter either:
   - `user@hostname` (for example, `netid@login.cluster.example.edu`), or
   - The full `ssh` command you normally run (for example, `ssh -p 2200 user@hostname`).
4. VS Code will then ask which SSH config file to update. Choose the default (usually `~/.ssh/config` or the Windows equivalent).
5. VS Code writes the appropriate entry to the chosen config file.

You can later edit this file manually if you need to change options.

## Connecting to the remote Linux server

Once the extension and SSH config are set up, you can connect to the remote Linux server from VS Code.

### Connect using the status bar

1. Click the green remote status indicator in the bottom‑left corner of VS Code.
2. Select **Remote-SSH: Connect to Host...**.
3. Choose one of the configured hosts, such as `polaris`.
4. If prompted, enter your ALCF MobilePass+ token.

After connecting, VS Code runs in a remote context, and you can open folders and files that live on the remote Linux machine.

### Connect using the Command Palette

1. Open the **Command Palette** (`Ctrl+Shift+P`).
2. Run **Remote-SSH: Open SSH Host...**.
3. Enter `user@host-or-ip` if you have not configured `~/.ssh/config`, or choose a host from the list if you have.
4. Once connected, use **File > Open Folder...** to open a directory on the remote host where your project files reside.

The status bar will show that VS Code is connected to the remote host, and you will see separate sections for local and remote extensions in the Extensions view.

## Installing the C/C++ extension (remote context)

To work efficiently with C and C++ code on the remote Linux server, install Microsoft’s C/C++ extension **in the remote context**.

1. Connect to the remote host via Remote - SSH.
2. In the remote VS Code window, open the **Extensions** view (`Ctrl+Shift+X`).
3. Search for `C/C++` (publisher: Microsoft).
4. Install the **C/C++** extension; VS Code will install it where your code and compiler live (on the remote host).

## Loading compiler modules on the remote Linux server

Most HPC and research Linux systems use **Environment Modules** or **Lmod** to manage compilers and libraries. These provide commands like `module load gcc` or `module load clang` that modify your environment so the correct compiler is found first on your `PATH`.

> Note: Some sites use `module add` as an alias for `module load`. In this guide, `module load` is shown, but `module add` may behave the same on your system.

### Check available compiler modules

In the integrated terminal of the remote VS Code session (or any shell on the remote server):

```bash
module avail
```

This lists available modules, such as different versions of `g++`, `nvc++`, and `clang`.

You can also list currently loaded modules with:

```bash
module list
```

### Load a C++ compiler module

To load GCC (g++):

```bash
module load PrgEnv-gnu
```

To load Clang:

```bash
module load PrgEnv-nvhpc

```

After loading, verify which compiler is active:

```bash
which cc
CC -show
CC --version
```

## Configuring compilers for VS Code (remote Linux)

VS Code does not include compilers; it calls the compilers that are available in the remote Linux environment. On `Polaris` or `Aurora`, you will typically use **GCC** , **Clang**, and **OneApi**. See compiler documentation for [Polaris](https://docs.alcf.anl.gov/polaris/compiling-and-linking/) and [Aurora](https://docs.alcf.anl.gov/aurora/compiling-and-linking/)

### Steps (remote side)

1. **Connect** to the remote Linux host using Remote - SSH in VS Code.
2. **Load the desired compiler module** in a terminal inside VS Code, for example:

   ```bash
   module load PrgEnv-gnu
   # or:
   module load PrgEnv-nvhpc # on Polaris
   # or:
   module oneapi # on Aurora
   ```

3. **Open your C++ project folder** on the remote host in VS Code.
4. Use **Terminal > Run Build Task...** or `Ctrl+Shift+B`. The C/C++ extension can:
   - Detect `CC` or `mpicxx` in your environment.
   - Offer to create a default `tasks.json` that compiles the active file.

A typical GCC‑based task in `.vscode/tasks.json` looks like this (works for both module‑provided GCC and a system GCC):

```json
{
  "version": "2.0.0",
  "tasks": [
    {
      "type": "cppbuild",
      "label": "C/C++: g++ build active file",
      "command": "CC",
      "args": [
        "-fdiagnostics-color=always",
        "-g",
        "${file}",
        "-o",
        "${fileDirname}/${fileBasenameNoExtension}"
      ],
      "options": {
        "cwd": "${fileDirname}"
      },
      "problemMatcher": ["$CC"],
      "group": {
        "kind": "build",
        "isDefault": true
      },
      "detail": "Task generated by Debugger."
    }
  ]
}
```

To use **OneApi** instead, set `"command": "mpicxx"` and adjust any flags as needed; the module ensures the correct `mpicxx` is on your `PATH`.

## Example: Creating and compiling `helloworld.cpp` (remote Linux)

The following example assumes:

- Your **local** machine is Windows, macOS, or Linux.
- You have connected to a **remote Linux** server with Remote - SSH.
- The remote system provides compilers via modules (for example, `gcc` or `clang` modules).

### 1. Open a project folder on the remote host

1. In the remote VS Code window, select **File > Open Folder...**.
2. Choose or create a folder, for example: `~/cpp-hello` on the remote Linux host.

### 2. Create `helloworld.cpp`

In the Explorer (remote window), create a new file named `helloworld.cpp` with:

```cpp
#include <iostream>

int main() {
    std::cout << "Hello, world!" << std::endl;
    return 0;
}
```

### 3. Load a compiler module on the remote host

Open a terminal in VS Code (this terminal runs on the remote Linux server), then load a compiler module:

```bash
# Example: load GCC
module load PrgEnv-gnu
# or:
module load PrgEnv-nvhpc # on Polaris
# or:
module oneapi # on Aurora
```

Check that the compiler is available:

```bash
CC --version
```

Environment Modules ensure these commands point to the compiler version selected by the module.

### 4. Configure a build task

1. In VS Code, open the **Command Palette** (`Ctrl+Shift+P`).
2. Run **Tasks: Configure Default Build Task** or **C/C++: Edit Configurations (UI)**.
3. If prompted, select `g++` (GCC) or `clang++` (Clang) as your compiler.
4. VS Code will create a `.vscode/tasks.json` see example above.
ags.

### 5. Build the program (on remote Linux)

1. Make sure `helloworld.cpp` is the active editor tab.
2. Run the default build task:
   - Press `Ctrl+Shift+B`, or
   - Use **Terminal > Run Build Task...** and select the `C/C++: ... build active file` task.
3. VS Code will invoke `g++` or `clang++` (provided by the loaded module) and produce an executable named `helloworld` in the same folder.
4. Build output and any compiler errors appear in the integrated terminal.

### 6. Run the program (on remote Linux)

From the same terminal (still on the remote Linux host):

```bash
./helloworld
```

You should see:

```text
Hello, world!
```

### 7. Optional: Configure debugging

1. Open the **Run and Debug** view (play icon with bug).
2. Click **create a launch.json file** and choose a C++ configuration, such as `(gdb) Launch`.
3. Confirm that `program` points to the compiled executable on the remote Linux host, for example:

   ```json
   "program": "${fileDirname}/${fileBasenameNoExtension}"
   ```

4. Set a breakpoint in `helloworld.cpp` by clicking in the gutter next to a line.
5. Press **F5** to start debugging; execution stops at the breakpoint and you can step through the program and inspect variables.

With these steps, users can:

- Run VS Code locally on Windows, macOS, or Linux.
- Connect to your Linux compute server over SSH.
- Use `module load` / `module add` to select compilers on the remote system.
- Compile and debug C++ programs such as `helloworld.cpp` entirely on the remote Linux host.
