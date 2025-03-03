# Continuous Integration via GitLab-CI For Aurora

### As of Febuary 25 2025, no changes from the [general documentation](https://docs.alcf.anl.gov/services/gitlab-ci/) for GitLab-CI are required for Aurora.


### The below information is *only* for those users who have projects created prior to that date and that still use `gitlab-sunspot.alcf.anl.gov`:


Currently, [https://gitlab-sunspot.alcf.anl.gov](https://gitlab-sunspot.alcf.anl.gov) must be accessed via a proxy.


The following command will connect to an Aurora login node from your local system and establish the required proxy:[^1]
```bash linenums="1"
ssh aurora.alcf.anl.gov -D localhost:25565
```

## Instructions for Firefox Browser

In order to use the proxy, you must configure your local web browser to use a SOCKS proxy. The instructions for other browsers are similar.

1. Open Firefox settings
2. Navigate to "General" > "Network Settings" > "Settings" 
    <small>(at the bottom of the General settings page.)</small>
3. Ensure "Manual proxy configuration" is selected
4. Fill the "SOCKS Host" field with `localhost`
5. Fill the associated port field with `25565` (or the alternate port you specified in your SSH command)
6. Ensure "SOCKS v5" is selected
7. Ensure "Proxy DNS when using SOCKS v5" is selected"
8. Select "OK"

!!! warning 

	You will not have internet access in Firefox while using the proxy. Select "No proxy" to re-enable internet access.

For ease of use, many users have had success using extensions like FoxyProxy, or using a separate web browser for accessing resources that require the proxied connection.


[^1]: `25565` is the proxy port, it may be changed as needed.
