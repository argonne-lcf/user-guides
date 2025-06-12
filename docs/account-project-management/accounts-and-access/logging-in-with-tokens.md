# Logging In with a Token

Once you have [obtained a token](obtaining-a-token.md), you can use it to log in to ALCF systems. 

You can log in with a [Mobile Token](#logging-in-to-an-alcf-system-using-a-mobile-token) using your mobile device. 

You can log in with a [Physical Token](#logging-in-to-an-alcf-system-using-a-physical-token) if you have received one in the mail.

## Logging in to an ALCF System using a Mobile Token

1. Open the MobilePASS+ app on your device.  
    Then initiate an SSH session and type the following:

    ```bash
    ssh <ALCF username>@<system_name>.alcf.anl.gov
    ```

    For example, <johnsmith@aurora.alcf.gov>

1. When prompted for a password, click the SafeNet MobilePASS+ app on your
   phone.

    Click on the token name listed within the app and enter your PIN.

1. The app will display your passcode immediately.

    Enter the passcode as the login password for the system within the SSH session.

    !!! info "Note"

        You do NOT have to enter the PIN on the SSH screen when logging into a
        resource.
        This only needs to be done to access the passcode within the SafeNet
        MobilePASS+ app.

1. Each generated passcode is valid on the SafeNet MobilePass+ app window until
   your mobile device screen times out.

## Logging in to an ALCF System using a Physical Token

When the physical token is activated, an initial PIN will be provided.

Upon **INITIAL** login (to one of the ALCF machines), a prompt to change the
PIN will appear.

PINs must be at least four characters long and must only contain
numbers.

1. Initiate an SSH session using:

    ```bash
    ssh <ALCF username>@<system_name>.alcf.anl.gov
    ```

2. A password prompt will be received.  
   At this point, push the button on the physical token once.

3. An eight-character, one-time password made up of letters and numbers will
   appear on the token’s display.
   This one-time password is case-sensitive.

4. Type your PIN followed immediately by the one-time password at the SSH
   password prompt.

   For example, if your PIN is 1234 and you received the one-time password
   string ABCD9876, you would type 1234ABCD9876 at the password prompt.
