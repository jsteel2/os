BIN=kernel.elf
OBJ=$(shell echo *.S *.c | sed 's/macros\.S //;s/ /.o /g;s/$$/.o/')
TRIPLET=riscv64-unknown-elf
CC=$(TRIPLET)-gcc
AS=$(TRIPLET)-as
LD=$(TRIPLET)-ld
DBG=$(TRIPLET)-gdb
DEBUG?=false
CFLAGS=-Wall -Wextra `$(DEBUG) && echo -Og -g || echo -O3` -mcmodel=medany -ffreestanding -I../dtc/libfdt -I../opensbi/include/sbi
ASFLAGS=`$(DEBUG) && echo -g`
LDFLAGS=-T linker.ld -nostdlib
EMU=qemu-system-riscv64
CORES?=4
MEM?=128M
DISK=disk.img
EMUFLAGS=-bios ../opensbi/build/platform/generic/firmware/fw_jump.elf -M virt -smp $(CORES) -m $(MEM) -nographic `$(DEBUG) && echo -s -S` -kernel $(BIN) -drive file=$(DISK),if=none,format=raw,id=x0 -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0

all: $(BIN)

%.S.o: %.S
	$(AS) $(ASFLAGS) -c -o $@ $<

%.c.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BIN): $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $(OBJ) ../dtc/libfdt/libfdt.a

$(DISK):
	qemu-img create -f raw $@ 1M

.PHONY: clean run

clean:
	rm -f $(BIN) $(OBJ) $(DISK)

run: $(BIN) $(DISK)
	$(DEBUG) && sh -c '$(EMU) $(EMUFLAGS) & $(DBG) -ex "target remote :1234" -ex "set confirm off" -ex "add-symbol-file $(BIN)"; kill $$!' || $(EMU) $(EMUFLAGS)
