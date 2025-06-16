# Troubleshooting Tokens

This page offers guidance on common issues users have with their tokens. 

You can view the common issues and fixes for [mobile tokens](#troubleshooting-your-mobile-token) and for [physical tokens](#troubleshooting-your-physical-token) below.

If your issue is not on this page, please contact [support@alcf.anl.gov](mailto:support@alcf.anl.gov).

## Troubleshooting your Mobile Token

- **Forgotten PIN:** If you enter a PIN for your mobile token and you get an
  invalid PIN, you will be asked to re-enter your PIN.
  After 6 failed attempts, your token will be deleted.
  If your token is deleted, call or send an email to ALCF support to have a new
  mobile token provisioned.
- **Account Lockout:** If you fail to enter the correct password 6 times, you
  will get a permission denied error on the SSH screen.
  If you fail to enter the correct password 4 more times, your IP will be
  blocked.
  You will need to email (<CALL>?) ALCF support and submit a ticket to have the
  IP unblocked.
- **PIN Change:** While logged in to the mobile token, click on token settings,
  then tap change PIN.
  Enter the current PIN followed by the new PIN and confirm.
- **Re-Sync:** If you are unable to log in to a resource after entering the
  correct PIN and passcode, your token may be out of sync with the server.
  Please email ALCF Service Desk at
  [accounts@alcf.anl.gov](mailto:accounts@alcf.anl.gov) for assistance.
- **New Mobile Device:** If you have a new mobile device, please email the ALCF
  Service Desk at [accounts@alcf.anl.gov](mailto:accounts@alcf.anl.gov) to have a
  new mobile token provisioned.

## Troubleshooting Your Physical Token

- **The token says "locked":** The physical token may be locked due to too many
  failed attempts.
  Please contact the ALCF Help Desk to return the defective token so a
  replacement can be sent.
- **You forgot your PIN:** Once a PIN has been set for your physical token, you
  will need to prepend your PIN to the token password.
  Otherwise, you will not be able to log in.
  If you do not remember your PIN, please email us so we can verify your
  identity and reset your initial PIN.
- **The physical token does not say "locked" but still does not work:** It is
  likely that your token has fallen out of sync with the server.
  If you have pushed the button on your physical token more than 10 times
  without successfully logging in, it will fail to authenticate because it has
  lost synchronization with the server.
  Please try connecting to the system first. If it still fails, please follow
  the re-sync instructions below.

### Re-Sync

If you have pushed the button on your physical token more than 10 times, it
will fail to authenticate because it has lost synchronization with the server.

You can re-synchronize your token using the following procedure:

1. Have your physical token ready.
2. Obtain a challenge sequence:
   - Initiate an SSH session to a host that allows token authentication (such
     as `polaris.alcf.anl.gov`).
     At the password prompt, just hit “Enter”.
     This will cause the physical token to produce a challenge string
     consisting of 8 numbers.
3. Hold down the button on your token for a few seconds until the display says
   "Init," then let go.
4. The token will scroll through a series of menu options. When it displays
   "ReSync," hit the button again.
5. The display will say “Resync?0”
6. The number at the end will start cycling from 0 to 9, over and over.
7. Look at the numbers in your challenge string.
   When the number displayed on your token changes to the first number of the
   challenge string, press the button. The display will now show this number,
   and the second digit will start cycling.
8. Enter each of the numbers from your challenge string in the same manner,
   until the display on your token matches the entire challenge string.
   Choose the "<" to backspace and re-enter the previous number if necessary.
9. Once you've entered all 8 digits, re-check to make sure they're accurate.
   Then, while all 8 digits are displayed on the token, press the button to
   generate a new password.
10. Enter your PIN followed by the new password, and hit 'Enter'.
   If successful, you will be logged in to the resource. You're now back in
   sync with the authentication server.

If you are unsuccessful, you will be presented with another challenge string.
At this point, you may need to perform the re-sync instructions again.

!!! warning "Unsuccessful Re-Sync"

    If there are still problems after completing the re-synchronization procedures,
    please email us at [accounts@alcf.anl.gov](mailto:accounts@alcf.anl.gov) so
    we can run a test on the physical token to determine if it is defective.
    If it is found to be defective, we will promptly replace it.

### Resetting the Physical Token PIN

Please email us at
[accounts@alcf.anl.gov](mailto:accounts@alcf.anl.gov)
for PIN resets.

Once your identity has been verified, we will provide you with a new PIN for
your physical token.
