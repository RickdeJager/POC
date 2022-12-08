# You probably want to edit this. Using your package managers toolchain is not recommended,
# As the devices libc version is probably older than you local version.
TOOLCHAIN_PRE = /opt/toolchains/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabi/bin/arm-linux-gnueabi-
CC = $(TOOLCHAIN_PRE)gcc
STRIP = $(TOOLCHAIN_PRE)strip

shell: shell.c
	$(CC) shell.c -o shell
	$(STRIP) shell
