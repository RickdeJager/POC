# Netgear RAX30 V1.0.7.78
These were our LAN bugs for Pwn2own Toronto 2022. They got patched a day before the submission deadline, but that was to be expected as both bugs are extremely shallow.

## Shell
The example uses the second stage shell that is included in this repo. To build it, either download the same toolchain used in the Makefile, or use your package manager to find a similar chain. If your toolchain is newer, you probably want to build the implant statically.

## UPNP
This one should "just work". You may have to disconnect from other networks to make sure the discovery process works correctly.

## urlfilterd
We used this bug as a "development" bug because it had some weird preconditions that we weren't sure ZDI would be okay with. It was still useful to get shell on the device for testing.  

Go to Advanced > Security > Block Sites and:
 - Select "Always"
 - Add a new keyword (anything will do)
 - Apply the settings
