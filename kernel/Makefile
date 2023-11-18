BIN=kernel.elf
OBJ=$(shell echo *.S *.c | sed 's/macros\.S //;s/ /.o /g;s/$$/.o/')
TRIPLET=riscv64-unknown-elf
CC=$(TRIPLET)-gcc
AS=$(TRIPLET)-as
LD=$(TRIPLET)-ld
DBG=$(TRIPLET)-gdb
CFLAGS=-Wall -Wextra `$(DEBUG) && echo -Og -g || echo -O3` -mcmodel=medany -ffreestanding -I../dtc/libfdt -I../opensbi/include/sbi
ASFLAGS=`$(DEBUG) && echo -g`
LDFLAGS=-T linker.ld -nostdlib
DEBUG?=false
EMU=qemu-system-riscv64
CORES?=1
EMUFLAGS=-bios ../opensbi/build/platform/generic/firmware/fw_jump.elf -M virt -smp $(CORES) -nographic `$(DEBUG) && echo -s -S` -kernel $(BIN) 

all: $(BIN)

%.S.o: %.S
	$(AS) $(ASFLAGS) -c -o $@ $<

%.c.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BIN): $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $(OBJ) ../dtc/libfdt/libfdt.a

.PHONY: clean run

clean:
	rm -f $(BIN) $(OBJ)

run: $(BIN)
	$(DEBUG) && sh -c '$(EMU) $(EMUFLAGS) & $(DBG) -ex "target remote :1234" -ex "set confirm off" -ex "add-symbol-file $(BIN)"; kill $$!' || $(EMU) $(EMUFLAGS)
