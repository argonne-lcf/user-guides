# User Authentication Policy

Users of the ALCF systems are required to use a SafeNet token (physical or mobile) one-time password, multifactor authentication system.

This document explains the policies users must follow regarding SafeNet tokens for accessing the ALCF systems.

## MultiFactor Authentication

"Authentication systems are frequently described by the authentication factors that they incorporate. The three factors often considered as the cornerstone of authentication are: Something you know (for example, a password); Something you have (for example, an ID badge or a cryptographic key); and Something you are (for example, a voice print or other biometric measurement)." -- NIST ITL Bulletin, Aug 2004

By the NIST guidelines for identification and authentication (NIST 800-53, Revision 3, Control IA-2), ALCF aims for a Moderate level of security controls. All production systems in ALCF require multifactor authentication for users with network and local (privileged and non-privileged accounts) using the SafeNet tokens.

## Mobile and Physical Tokens

ALCF provides every user of the production resources a physical or mobile token called a SafeNet Token. This is named after the company that developed the key fob and mobile software (the organization is now called SafeNet). "Both tokens use AES-256 bit encryption to generate OTPs [One-Time Passwords] comprised of digits, digits and letters or digits, letters and special characters..."

When you receive your physical token, it will be initialized, but it will have no access privileges until you have contacted us to verify your identity.

At the end of your account or project lifecycle, please return the token to the ALCF help desk:

ALCF Service Desk  
Argonne National Laboratory  
9700 South Cass Avenue  
Building 240  
Argonne, IL 60439

## Protect Your Passcode Token

Your passcode token should be protected by you as carefully as your credit cards or house keys. If your token is lost, stolen, or damaged, please contact us immediately so that we can deactivate the token and prevent unauthorized access. Sharing of tokens is strictly forbidden. Please do not mark on the token or alter it in any way.

## More Information

[New User Guide](http://www.alcf.anl.gov/user-guides/new-user-guide)

[Using Passcode Tokens](https://www.alcf.anl.gov/support-center/account-and-project-management/alcf-passcode-tokens)

## References

- [http://www.itl.nist.gov/lab/bulletns/bltnaug04.htm](http://www.itl.nist.gov/lab/bulletns/bltnaug04.htm)
- [http://csrc.nist.gov/publications/nistpubs/800-53-Rev3/sp800-53-rev3-final_updated-errata_05-01-2010.pdf](http://csrc.nist.gov/publications/nistpubs/800-53-Rev3/sp800-53-rev3-final_updated-errata_05-01-2010.pdf)
- [http://csrc.nist.gov/publications/nistpubs/800-63-1/SP-800-63-1.pdf](http://csrc.nist.gov/publications/nistpubs/800-63-1/SP-800-63-1.pdf)
- [https://safenet.gemalto.com/multi-factor-authentication/authenticators/one-time-password-otp/](https://safenet.gemalto.com/multi-factor-authentication/authenticators/one-time-password-otp/)