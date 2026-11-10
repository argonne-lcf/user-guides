---
description: "Configure your local SSH config and use X11 forwarding to run graphical apps from ALCF login nodes."
tags:
  - Development Environment
  - Debugging
keywords:
  - ssh
  - X11
  - XQuartz
  - X forwarding
  - ForwardX11
---

# Configure SSH and X11 Forwarding

Configure your local SSH once to connect to ALCF login nodes with short names like `ssh aurora`, then optionally enable X11 forwarding for graphical applications. No VPN is required to reach ALCF systems from the internet; connect directly with SSH over the public network.

/// note | No VPN required
You do not need a VPN to SSH to ALCF login nodes. Connect directly from your local machine with `ssh <username>@aurora.alcf.anl.gov` or the short names configured below. If your institution routes SSH through a jump host or VPN, that is a local policy, not an ALCF requirement.
///

/// tip | Do you need X11 forwarding?
Many workflows have better alternatives: [VS Code Remote - SSH](vscode.md) for editing, Jupyter for notebooks, or VNC for persistent desktops. Use X11 forwarding for lightweight tools that require a display, such as `xterm`, quick ParaView tests, or graphical installers. For heavy visualization, launch ParaView or VisIt from a client as described in the system visualization pages.
///

## Configure Your Local SSH Config File

Edit the SSH config file on your local machine, not on the ALCF system. This file controls how your local `ssh` behaves.

- **macOS / Linux:** `~/.ssh/config`
- **Windows (OpenSSH):** `C:\Users\<username>\.ssh\config`

Set permissions so only you can read and write it:

```bash
chmod 700 ~/.ssh
chmod 600 ~/.ssh/config
```

Recommended ALCF example using one block for multiple hosts. Replace `<username>` with your ALCF username. `%h` expands to the host you typed (`aurora`, `polaris`, or `crux`), so you do not need a separate `HostName` per system:

```bash linenums="1" title="~/.ssh/config"
# ALCF SSH config — keep this block separate from other Host entries
Host aurora polaris crux
  HostName %h.alcf.anl.gov              # %h expands to the host you typed (aurora, polaris, crux)
  User <username>                       # your ALCF username
  ForwardX11 yes                        # enable X11 forwarding (see X11 Forwarding below)
  ForwardX11Trusted yes                 # use trusted forwarding; avoids failures with some X toolkits
  ControlMaster auto                    # reuse one authenticated connection for additional sessions
  ControlPersist 45m                    # keep the master open 45 minutes after the last session closes
  ControlPath %d/.ssh/multiplex         # socket location for the shared connection; %d is your home directory
```

Create the multiplex directory once if it does not exist (the `ControlPath` directory must exist):

```bash
mkdir -p ~/.ssh/multiplex
chmod 700 ~/.ssh/multiplex
```

Then connect with the short name:

```bash
ssh aurora
```

```bash
ssh polaris
```

Disable forwarding for one connection without editing the file:

```bash
ssh -x aurora
```

/// note | Placeholder names
Commands run on your laptop use `<username>` as a placeholder. `$USER` there is your local username, not your ALCF username, so keep `<username>` in those examples. In commands run on an ALCF login node, `$USER` or `$HOME` already have the right values.
///

/// details | About ControlMaster / ControlPersist
    type: info
`ControlMaster auto` reuses one authenticated connection for additional tabs to the same host so you authenticate only once. `ControlPersist 45` keeps the master open for 45 seconds after the last session closes (use `45m` for 45 minutes). `ControlPath` is where the socket lives — `%d` is your home directory.

If your local OpenSSH is older (notably the system OpenSSH on some Windows installs), `ControlMaster`/`ControlPersist`/`ControlPath` may be unsupported — remove those three lines and keep `ForwardX11`/`ForwardX11Trusted`. They are compatible with current macOS and Linux OpenSSH.
///

## Per-System Examples

These snippets are interchangeable. Add the ones you use to the same `~/.ssh/config` file. See the hostnames for each system on its Getting Started page: [Aurora](../aurora/getting-started-on-aurora.md) and [Polaris](../polaris/getting-started.md).

/// tab | Aurora
```bash linenums="1" title="~/.ssh/config — Aurora"
# ALCF SSH config — Aurora hosts
Host aurora aurora-login
  HostName %h.alcf.anl.gov              # %h expands to aurora or aurora-login
  User <username>                       # your ALCF username
  ForwardX11 yes                        # enable X11 forwarding
  ForwardX11Trusted yes                 # trusted forwarding (recommended for ALCF)
  ControlMaster auto                    # reuse authenticated connection
  ControlPersist 45m                    # keep the master open 45 minutes after the last session closes
  ControlPath %d/.ssh/multiplex         # socket location for the shared connection; %d is your home directory
```

```bash
ssh -Y aurora
```
///

/// tab | Polaris
```bash linenums="1" title="~/.ssh/config — Polaris"
# ALCF SSH config — Polaris hosts
Host polaris polaris-login
  HostName %h.alcf.anl.gov              # %h expands to polaris or polaris-login
  User <username>                       # your ALCF username
  ForwardX11 yes                        # enable X11 forwarding
  ForwardX11Trusted yes                 # trusted forwarding (recommended for ALCF)
  ControlMaster auto                    # reuse authenticated connection
  ControlPersist 45m                    # keep the master open 45 minutes after the last session closes
  ControlPath %d/.ssh/multiplex         # socket location for the shared connection; %d is your home directory
```

```bash
ssh -Y polaris
```
///

/// tab | No config file (flags only)
If you do not want a config file, pass the flag each time (trusted forwarding is recommended for ALCF hosts):

```bash
ssh -Y <username>@aurora.alcf.anl.gov
```

```bash
ssh -Y <username>@polaris.alcf.anl.gov
```
///

/// tab | Sophia and Crux
The same pattern applies to other ALCF systems. Use the hostname listed in the system documentation:

```bash linenums="1" title="~/.ssh/config — additional systems"
# ALCF SSH config — additional systems
Host sophia crux
  HostName %h.alcf.anl.gov              # %h expands to sophia or crux
  User <username>                       # your ALCF username
  ForwardX11 yes                        # enable X11 forwarding
  ForwardX11Trusted yes                 # trusted forwarding (recommended for ALCF)
  ControlMaster auto                    # reuse authenticated connection
  ControlPersist 45m                    # keep the master open 45 minutes after the last session closes
  ControlPath %d/.ssh/multiplex         # socket location for the shared connection; %d is your home directory
```
///

## X11 Forwarding

X11 forwarding lets a graphical application started on an ALCF login node display on your local machine. It is optional — configure it only if you need lightweight X clients. For editors, notebooks, or persistent desktops, prefer [VS Code Remote - SSH](vscode.md), Jupyter, or VNC.

### Prerequisites on Your Local Machine

Install and start a local X server. You need this only where you sit, not on Aurora or Polaris. Keep the X server running before you connect with `ssh -X` or `ssh -Y`.

/// tab | macOS
Download the installer from [XQuartz](https://www.xquartz.org/) and run it, then restart your Mac so `DISPLAY` is set. Alternatively, if you use Homebrew:

```bash
brew install --cask xquartz
```

After install, launch XQuartz, then in XQuartz go to **Settings > Security** and check **Allow connections from network clients** if an application requires trusted forwarding.

Verify `xauth` and `DISPLAY` locally before connecting:

```bash
which xauth
```

```bash
echo $DISPLAY
```

``` { .output .no-copy }
`/private/tmp/com.apple.launchd..../org.xquartz:0`
```

Start XQuartz first, then open a terminal and `ssh`. If you start `ssh` from Terminal.app without XQuartz running, `DISPLAY` is not set and forwarding will fail.
///

/// tab | Linux
Most Linux desktops already run an X server or XWayland. No extra install is needed. Verify `DISPLAY` locally:

```bash
echo $DISPLAY
```

``` { .output .no-copy }
`:0`
```

Check that `xauth` exists:

```bash
which xauth
```
///

/// tab | Windows
Use an X server implementation:

- **MobaXterm** (recommended) has a built-in X server and enables X11 forwarding by default. Check **Settings > Configuration > X11 > X11 Forwarding** is enabled.
- **VcXsrv** or **Xming** with the built-in OpenSSH client or PuTTY. In PuTTY, enable **Connection > SSH > X11 > Enable X11 forwarding**. With OpenSSH on Windows, use the same `ssh -X` flags and `~/.ssh/config` file at `C:\Users\<username>\.ssh\config`.
///

Remote side requirements are already present on ALCF login nodes: `xauth` is installed and `sshd` allows forwarding. If `ssh -v -X` reports `X11 forwarding request failed`, try a different login node and contact support with the verbose output.

### X11 Forwarding Options

| Mode | Command-line flag | `~/.ssh/config` setting | When to use |
|---|---|---|---|
| Enable trusted forwarding | `ssh -Y <username>@aurora.alcf.anl.gov` | `ForwardX11 yes` and `ForwardX11Trusted yes` | Recommended for ALCF systems. Allows full X operations. |
| Enable untrusted forwarding | `ssh -X <username>@aurora.alcf.anl.gov` | `ForwardX11 yes` and `ForwardX11Trusted no` | Fallback if you prefer stricter X security controls. |
| Disable forwarding | `ssh -x <username>@aurora.alcf.anl.gov` | `ForwardX11 no` | Override a config that enables forwarding for a single connection. |

/// tip | Use trusted forwarding for ALCF hosts
ALCF recommends trusted forwarding (`-Y` / `ForwardX11Trusted yes`) for ALCF login nodes. It avoids failures with some X toolkits. Scope it to ALCF hosts only — never set `ForwardX11 yes` or `ForwardX11Trusted yes` for `Host *`. A forwarded session lets the remote host interact with your local display until you disconnect.
///

You can inspect what SSH negotiated:

```bash
ssh -v -Y <username>@aurora.alcf.anl.gov exit
```

``` { .output .no-copy }
debug1: Requesting X11 forwarding with authentication spoofing.
debug1: X11 forwarding requested
```

No output about X11 means forwarding was not requested (check your config and flags). `X11 forwarding request failed on channel 0` means the server side refused it.

### Verify That Forwarding Works

After connecting with `ssh -Y`, check `DISPLAY` on the remote host. It should be set by SSH to a `localhost:10.0`-style value, not empty and not `:0`:

```bash
echo $DISPLAY
```

``` { .output .no-copy }
localhost:10.0
```

If `DISPLAY` is empty, you connected without forwarding. Exit and reconnect with `-Y`, or check the `ForwardX11` line in `~/.ssh/config`.

Open a simple X terminal to confirm the display forwards. Close it with the window button or `++ctrl+c++`:

```bash
xterm
```

If the `xterm` window appears on your local screen, forwarding works. Test again after adding the SSH config entry to confirm it applies without flags.

### Forwarding to Compute Nodes

You cannot SSH directly from your local machine to a compute node. You must first SSH to a login node, then hop to the compute node from there while your allocation is active.

Most visualization work should use a client-side launcher or VNC rather than X11 forwarding to a compute node. If you have an allocation and must test X11 on a compute node, request an interactive job from the login node, then SSH-hop to the assigned node from that login session with X11 forwarding:

```bash
# from your local machine to the login node (with X11 forwarding)
ssh -Y aurora
# then, from the login node to the compute node assigned to your job
ssh -Y <compute-node-hostname>
```

The compute-node hostname is provided by your scheduler job output (for example, in `qstat` or the interactive-job startup message). Do not attempt to SSH to compute nodes from outside ALCF or without an active allocation.

Consult the system-specific running-jobs pages for interactive syntax: [Aurora](../aurora/running-jobs-aurora.md) and [Polaris](../polaris/running-jobs/index.md).

## Troubleshooting

- **`cannot open display:` or `No protocol specified`** — X server not running locally, or you connected without `-Y`. Start XQuartz or your Windows X server first, then reconnect with `ssh -Y`. On macOS, launch XQuartz before `ssh` from Terminal.app.
- **`X11 forwarding request failed on channel 0`** — remote `xauth` missing or admin disabled `X11Forwarding`. Try another login host. Capture `ssh -v -Y` output and include it in a ticket to [Support](../support/ticket.md).
- **`xauth: file /home/$USER/.Xauthority does not exist`** — usually harmless; SSH creates it. If forwarding still fails, check `xauth list` on both ends.
- **`Authorization required, but no authorization protocol specified`** — stale local `~/.Xauthority`. On your local machine, regenerate it and reconnect:

```bash
xauth list
```

```bash
xauth generate $DISPLAY .
```

## Security Notes

- Trusted forwarding (`ForwardX11Trusted yes`) is recommended for ALCF hosts and is set in the example above. Do not set `ForwardX11 yes` or `ForwardX11Trusted yes` for `Host *` — scope both to ALCF hosts only (`Host aurora polaris crux`). A forwarded session lets the remote host interact with your local display until you disconnect, so log out when done.
- `ForwardAgent` is unrelated to X11. Leave it `no` unless you have a specific jump-host workflow that requires agent forwarding.
- `ControlMaster`/`ControlPersist` reuse one authenticated connection. Keep `ControlPath` under `~/.ssh/` with `chmod 700 ~/.ssh` so only you can access the socket. Remove `ControlMaster`/`ControlPersist`/`ControlPath` on systems where they are unsupported.
- If you use VS Code Remote - SSH for most work, keep its SSH config entries separate from X11 entries or note which hosts enable forwarding so you do not unintentionally forward everywhere. See [VS Code Remote - SSH](vscode.md) for its example config.

