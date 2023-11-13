BIN=kernel.elf
TRIPLET=riscv64-unknown-elf
AS=$(TRIPLET)-as
CC=$(TRIPLET)-gcc
LD=$(TRIPLET)-ld
OBJ=boot.o trap.o kmain.o page.o virt_enable.o virt.o kprint.o

CFLAGS=-Wall -Wextra -O3 -mcmodel=medany -ffreestanding
LDFLAGS=-T linker.ld -nostdlib

all: $(BIN)

%.o: %.S
	$(AS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BIN): $(OBJ)
	$(LD) $(LDFLAGS) $(OBJ) -o $@

.PHONY: clean run

clean:
	rm -f $(OBJ) $(BIN)

run: $(BIN)
	@echo '!!!!!!!!!!! INCASE YOU FORGET, ITS CTRL+A X TO EXIT !!!!!!!!!!!!!'
	qemu-system-riscv64 -kernel $(BIN) -bios none -machine virt -nographic -m 640
