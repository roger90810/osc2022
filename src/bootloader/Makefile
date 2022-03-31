CFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -mcpu=cortex-a53+nosimd
OBJS = bootloader.o uart.o queue.o
INCLUDES = -I ../uart.h ../gpio.h
all: clean bootloader.img

start.o: start.S
	clang --target=aarch64-elf $(CFLAGS) -c start.S -o start.o

uart.o: ../uart.c
	clang --target=aarch64-elf $(CFLAGS) -I ../uart.h -c ../uart.c -o uart.o

queue.o: ../queue.c
	clang --target=aarch64-elf $(CFLAGS) -I ../queue.h -c ../queue.c -o queue.o

bootloader.o: bootloader.c
	clang --target=aarch64-elf $(CFLAGS) -c bootloader.c -o bootloader.o

bootloader.img: start.o $(OBJS)
	ld.lld -m aarch64elf -nostdlib start.o $(OBJS) -T link.ld -o bootloader.elf
	llvm-objcopy -O binary bootloader.elf bootloader.img

run:
	qemu-system-aarch64 -M raspi3b -kernel bootloader.img -serial null -serial pty -initrd ../initramfs/initramfs.cpio -dtb ../devicetree/bcm2710-rpi-3-b-plus.dtb

debug:
	qemu-system-aarch64 -M raspi3b -kernel bootloader.img -serial null -serial pty -display none -S -s ../initramfs/initramfs.cpio -dtb ../devicetree/bcm2710-rpi-3-b-plus.dtb

clean:
	rm bootloader.elf bootloader.img *.o >/dev/null 2>/dev/null || true