BIN=kernel.elf
TRIPLET=riscv64-unknown-elf
AS=$(TRIPLET)-as
CC=$(TRIPLET)-gcc
LD=$(TRIPLET)-ld
DBG=$(TRIPLET)-gdb
DEBUG?=false
EMU=qemu-system-riscv64
EMUFLAGS=-bios none -machine virt -nographic -m 32 `$(DEBUG) && echo -s -S` -kernel
OBJ=boot.o trap_vector.o kmain.o page.o virt_enable.o virt.o kprint.o symbols.o trap.o plic.o uart.o kmem.o process.o user.o

CFLAGS=-Wall -Wextra `$(DEBUG) && echo -Og -g || echo -O3` -mcmodel=medany -ffreestanding
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
	@ $(DEBUG) || echo '!!!!!!!!!!! INCASE YOU FORGET, ITS CTRL+A X TO EXIT !!!!!!!!!!!!!'
	$(DEBUG) && sh -c '$(EMU) $(EMUFLAGS) $(BIN) & $(DBG) -ex "target remote :1234" -ex "set confirm off" -ex "add-symbol-file $(BIN)"; kill $$!' || $(EMU) $(EMUFLAGS) $(BIN)
