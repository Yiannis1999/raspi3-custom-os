# raspi3-custom-os
 A bare-metal OS/Application for Raspberry Pi 3

## Introduction
The OS looks for a file named "COUNTER" in the root folder of the FAT32 partitioned SD card. Then, it increases the number stored in the file by 1 and saves it. The file before and after the change is sent via UART.

## Features
- UART Communication
- SD Card I/O
- FAT32 File System
- Standard C Library

## Toolchain
Install aarch64-none-elf-gcc using the following script:
```sh
wget https://developer.arm.com/-/media/Files/downloads/gnu-a/10.3-2021.07/binrel/gcc-arm-10.3-2021.07-x86_64-aarch64-none-elf.tar.xz
tar -xf gcc-arm-10.3-2021.07-x86_64-aarch64-none-elf.tar.xz --directory /usr/local
for f in /usr/local/gcc-arm-10.3-2021.07-x86_64-aarch64-none-elf/bin/*
do
    n=${f##*/}
    update-alternatives --install /usr/bin/$n $n $f 1
done
```

## Emulation
Run qemu using:
```sh
qemu-system-aarch64 -M raspi3b -serial stdio -kernel kernel.elf -drive file=image.img,if=sd,format=raw
```

## Inspection
Mount the image using:
```sh
mkdir -p /mnt/image
mount image.img /mnt/image
```
Unmount using:
```sh
umount /mnt/image
rmdir /mnt/image
```

## Note
You may need root privileges.
