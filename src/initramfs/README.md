# initramfs
## How to create CPIO Archive.

1. You need to create a `rootfs` directory.
2. Put some files (plain text) in it.
3. Then run the following commands to create cpio archive.
```shell=
cd rootfs
find . | cpio -o -H newc > ../initramfs.cpio
cd ..
```

## Loading CPIO Archive

### QEMU
* Add the argument `-initrd <cpio archive>` to QEMU.
* QEMU loads the cpio archive file to `0x8000000` by default.
* In C Code, read address `0x8000000` to get the cpio info.

### Rpi3
* Move the cpio archive into SD card.
* Then specify the name and loading address in `config.txt`.
```
initramfs initramfs.cpio 0x20000000
```
* In C Code, read address `0x20000000` to get the cpio info.