SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
CFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -mcpu=cortex-a53+nosimd
PROJECT = program

all: clean $(PROJECT).img $(PROJECT).objdump copy

$(PROJECT).o: $(PROJECT).S
	clang --target=aarch64-elf $(CFLAGS) -c $< -o $@

%.o: %.c
	clang --target=aarch64-elf $(CFLAGS) -c $< -o $@

$(PROJECT).img: program.o $(OBJS)
	ld.lld -m aarch64elf -nostdlib program.o $(OBJS) -T link.ld -o $(PROJECT).elf
	llvm-objcopy -O binary $(PROJECT).elf $(PROJECT).img

$(PROJECT).objdump: $(PROJECT).elf
	objdump -d $(PROJECT).elf > $(PROJECT).objdump

clean:
	rm $(PROJECT).elf *.o >/dev/null 2>/dev/null || true

copy:
	cp $(PROJECT).img ../initramfs/rootfs