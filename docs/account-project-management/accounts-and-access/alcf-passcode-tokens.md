# ALCF Passcode Tokens

Please note: An account can be associated with a single token only (Mobile or Physical token). Please contact [accounts@alcf.anl.gov](mailto:accounts@alcf.anl.gov) to change your token preference.

## Mobile Token
The SafeNet MobilePass+ Mobile Token allows access to ALCF systems. This security mobile token uses one-time passwords combined with your PIN for controlled access to the login systems. The mobile token utilizes an app that is keyed to your user account and for which you are responsible on your Android, iPhone, or Windows mobile device. Please safeguard your phone as you would your credit cards or house keys: Do not store username, PIN, or other account-related records with the token. Sharing of mobile tokens is strictly forbidden. A mobile token can be associated with a single device only.

### Step 1. Download the SafeNet MobilePass+ app for your device:
The SafeNet MobilePASS+ app turns your device into a two-factor authentication device, removing the need to carry an additional hardware token. As a SafeNet MobilePASS+ user, you can generate passcodes on your mobile device and use those passcodes to authenticate on ALCF computing resources. See supported [OS and platforms](https://cpl.thalesgroup.com/access-management/authenticators/software-authentication/mobilepass-plus-push-authentication) for more information.

### Step 2. Enroll your MobilePass+ mobile token:
After you’ve been provisioned a mobile token, you will receive a notification email with the subject line "ALCF Mobile Token Self-Enrollment," which you must access from the device on which you wish to install the token.

**Auto-Enrollment** (to enroll SafeNet MobilePass+ token automatically):

1. Click on the http:// link in the email. The SafeNet Authentication Service Self-Enrollment will open.
2. Click enroll your SafeNet MobilePass+ token.
3. When prompted to open in MobilePass+, tap **Open**.
4. You will now be prompted to enter a 6-digit all-numeric PIN.
5. Enter your PIN in the **Token PIN** field and repeat in the **Confirm PIN** field.
6. You will be taken to the **Enrollment Complete** screen to name the token.
7. Insert the desired name in the **Token Name** field or leave it as is. This name is not utilized by the server; it is for you only.
8. The newly enrolled SafeNet MobilePass+ token is now displayed in the SafeNet MobilePass+ app.

**Manual Enrollment:**

1. Copy the activation string from the SafeNet provision email.
2. Open the SafeNet MobilePass+ app and tap the manual option.
3. Paste the enrollment string into the field provided and tap the Enroll button.
4. You will now be prompted to enter a 6-digit all-numeric PIN.
5. Enter your PIN in the Token PIN field and repeat in the Confirm PIN field.
6. You will be taken to the Enrollment Complete screen to name the token.
7. Insert the desired name in the Token Name field or leave it as is. This name is not utilized by the server; it is for you only.

## Logging in to an ALCF System using a Mobile Token

1. Open the MobilePASS+ app on your device. Then initiate an SSH session and type the following:

    ```bash
    ssh <ALCF username>@<system_name>.alcf.anl.gov
    ```

2. When prompted for a password, click the SafeNet MobilePASS+ app on your phone. Click on the token name listed within the app, and enter your PIN.

3. The app will display your passcode immediately. Enter the passcode as the login password for the system within the SSH session. Please **Note:** You do NOT have to enter the PIN on the SSH screen when logging into a resource. This only needs to be done to access the passcode within the SafeNet MobilePASS+ app.

4. Each generated passcode is valid on the SafeNet MobilePass+ app window until your mobile device screen times out.

## Troubleshooting your Mobile Token
**Case 1:** Forgotten PIN: If you enter a PIN for your mobile token and you get an invalid PIN, you will be asked to re-enter your PIN. After 6 failed attempts, your token will be deleted, and you will need to call the ALCF help desk or send an email to ALCF support to have a new mobile token provisioned.

**Case 2:** Account Lockout: If you fail to enter the correct password 6 times, you will get a permission denied error on the SSH screen. Upon 4 more failed attempts, your IP will be blocked. You will need to call the ALCF help desk and submit a ticket to have the IP unblocked.

**Case 3:** PIN Change: While logged in to the mobile token, click on token settings, then tap change PIN. Enter the current PIN followed by the new PIN and confirm.

**Case 4:** Re-Sync: If you are unable to log in to a resource after entering the correct PIN and passcode, your token may be out of sync with the server. Please email ALCF Service Desk at accounts@alcf.anl.gov for assistance.

**Case 5:** New Mobile Device: If you have a new mobile device, please email the ALCF Service Desk at accounts@alcf.anl.gov to have a new mobile token provisioned.

## Physical Token
The physical token allows access to the ALCF systems. This security token uses one-time passwords combined with your PIN for controlled access to the login systems. The physical token is a tracked asset for which you are responsible and is keyed to your use. Please safeguard your token as you would your credit cards or house keys: Do not store username, PIN, or other account-related records with the token. Sharing of tokens is strictly forbidden. Please do not mark on the token or alter it in any way.

## Enabling Your ALCF Physical Token
Upon receipt of the CRYPTOCard token, contact [accounts@alcf.anl.gov](mailto:accounts@alcf.anl.gov) to verify your identity and activate the token. If this step is not performed, the CRYPTOCard token will not be able to log on to the ALCF resource.

**ALCF Accounts Service Desk Info**
**Hours:** Monday-Friday 9 a.m. - 5 p.m. (Central time);
**Email:** [accounts@alcf.anl.gov](mailto:accounts@alcf.anl.gov)

## Logging in to an ALCF System using a Physical Token
When the physical token is activated, an initial PIN will be provided. This will be a four-digit number that will prepend to the one-time password string generated by the token.

Upon **INITIAL** login (to one of the ALCF machines), a prompt to change the PIN will appear. PINs must be at least four characters long and must only contain numbers.

1. Initiate an SSH session using:

    ```bash
    ssh <ALCF username>@<system_name>.alcf.anl.gov
    ```

2. A password prompt will be received. At this point, push the button on the physical token once.

3. An eight-character, one-time password made up of letters and numbers will appear on the token’s display. This one-time password is case-sensitive.

4. Type your PIN followed immediately by the one-time password at the SSH password prompt.

For example, if your PIN is 1234 and you received the one-time password string ABCD9876, you would type 1234ABCD9876 at the password prompt.

## Troubleshooting Your Physical Token
**Case 1:** It says "locked": The physical token may be locked due to too many failed attempts. Please contact the ALCF Help Desk to return the defective token so a replacement can be sent.

**Case 2:** You have a PIN for your physical token: Once a PIN has been set for your physical token, you will need to prepend your PIN to the token password. Otherwise, you will not be able to log in. If you do not remember your PIN, please email us so we can verify your identity and reset your initial PIN.

**Case 3:** It does not say "locked" but still does not work: It is likely that your token has fallen out of sync with the server. If you have pushed the button on your physical token more than 10 times without successfully logging in, it will fail to authenticate because it has lost synchronization with the server. Please try connecting to Polaris first. If it still fails, please follow the re-sync instructions below.

## Re-Sync Instructions

If you have pushed the button on your physical token more than 10 times, it will fail to authenticate because it has lost synchronization with the server. You can re-synchronize your token using the following procedure:

1. Have your physical token ready.

2. Obtain a challenge sequence:
    - Initiate an SSH session to a host that allows token authentication (such as polaris.alcf.anl.gov). At the password prompt, just hit 'Enter'. This will cause the Cryptocard service to produce a challenge string consisting of 8 numbers.

3. Hold down the button on your token for a few seconds until the display says "Init," then let go.

4. The token will scroll through a series of menu options. When it displays "ReSync," hit the button again.

5. The display will say

    Resync?0

6. The number at the end will start cycling from 0 to 9, over and over.

7. Look at the numbers in your challenge string. When the number displayed on your token changes to the first number of the challenge string, press the button. The display will now show this number, and the second digit will start cycling.

8. Enter each of the numbers from your challenge string in the same manner, until the display on your token matches the entire challenge string. Choose the "<" to backspace and re-enter the previous number if necessary.

9. Once you've entered all 8 digits, re-check to make sure they're accurate. Then, while all 8 digits are displayed on the token, press the button to generate a new password.

10. Enter your PIN followed by the new password, and hit 'Enter'. If successful, you will be logged in to the resource. You're now back in sync with the authentication server.

If you are unsuccessful, you will be presented with another challenge string. At this point, you may need to perform the re-sync instructions again.

If there are still problems after completing the re-synchronization procedures, please email us at [accounts@alcf.anl.gov](mailto:accounts@alcf.anl.gov) so we can run a test on the physical token to determine if it is defective.

If it is found to be defective, we will promptly replace it. Physical tokens are the property of Argonne National Laboratory.

Please return them to us at:

```
ALCF Help Desk
Argonne National Laboratory
9700 S. Cass Ave.
Bldg. 240, Rm. 2129
Lemont, IL 60439
```

## Resetting the Physical Token PIN
Please email us at support@alcf.anl.gov for PIN resets. Once your identity has been verified, we will provide you with a new PIN for your CRYPTOcard token.

## Returning a Physical Token
If you no longer need your physical token, please return it to this address:

```
ALCF Help Desk
Argonne National Laboratory
9700 S. Cass Ave.
Bldg. 240, Rm. 2129
Lemont, IL 60439
```