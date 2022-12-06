# Visit on Cooley
## Getting Started

On your local machine:

  - Download ([https://wci.llnl.gov/simulation/computer-codes/visit/downloads](https://wci.llnl.gov/simulation/computer-codes/visit/downloads)) and install VisIt (The most recent version installed on Cooley is 2.12.3. Versions 2.10.2 and  2.9.1 are also available.)
  - Download the [Cooley host profile for VisIt](https://www.alcf.anl.gov/support/user-guides/cooley/software-and-libraries/files/host_anl_cooley_1.xml) (you may need to right-click and choose "Save link as..." or "Save target as...")
  - Copy this file to a file called ~/.visit/hosts/host_anl_cooley.xml 

## Running VisIt in Interactive Mode
  - Start up VisIt on your local machine (Version 2.12.3, 2.10.2, or 2.9.1)
  - Click File -> Open File and choose "ANL Cooley" from the "Host" dropdown
  - You'll be prompted for your password; enter your Cryptocard response (with PIN)
  - When you open a selected file, it will launch a job on Cooley

    - You will need to specify the "Bank" (Project) to use when VisIt submits jobs to the queue on Cooley. Specify a project in the Options box.
    - If your environment doesn't get sourced correctly with non-interactive SSH, you can set the default project to use under Options -> Host profiles
    - **Note:** Don't change the contents of the "Machine file" field (it should be $COBALT_NODEFILE)
    - **Note:** The default Launch Profile is set to serial.  Do not change this default setting.
    - If you'd like to change other job parameters (like the number of processes, nodes, and walltime), you can do so.
    - If you'd like these changes to be used as your default, be sure to save them using Save Settings under the Options menu.

## Running VisIt in Batch Mode
  - Edit your .soft.cooley file to include the "@visit" key before the "@default" line 
  - When running VisIt in batch mode on Cooley, the default version is 2.12.3
  If you need a different version, you can specify this as a command line option using "-v <version number>" (versions 2.10.2 and 2.9.1 are also supported) 

## Additional Information
- Visit user manual: [https://wci.llnl.gov/codes/visit/manuals.html](https://wci.llnl.gov/codes/visit/manuals.html)
- Visit wiki: [http://www.visitusers.org](http://www.visitusers.org)
