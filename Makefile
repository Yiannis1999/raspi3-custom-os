SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

all: clean kernel8.img image.img

boot.o: boot.S
	aarch64-none-elf-gcc -c boot.S -o boot.o

%.o: %.c
	aarch64-none-elf-gcc -c $< -o $@

kernel.elf: boot.o $(OBJS)
	aarch64-none-elf-gcc -nostartfiles -T linker.ld -o kernel.elf boot.o $(OBJS) -Wl,--unresolved-symbols=ignore-all

kernel8.img: kernel.elf
	aarch64-none-elf-objcopy kernel.elf -O binary kernel8.img

image.img:
	dd if=/dev/zero of=image.img bs=1M count=1
	mkfs.fat -F 32 image.img
	mkdir -p /mnt/image
	mount image.img /mnt/image
	cp ./COUNTER /mnt/image
	umount /mnt/image
	rmdir /mnt/image

clean:
	rm image.img kernel8.img kernel.elf *.o || true
