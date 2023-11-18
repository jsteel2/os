all: opensbi libfdt kern

.PHONY: kern opensbi libfdt run clean

kern:
	$(MAKE) -C kernel

opensbi:
	$(MAKE) -C opensbi PLATFORM=generic CROSS_COMPILE=riscv64-linux-gnu-

libfdt:
	$(MAKE) -C dtc STATIC_BUILD=1 CC=riscv64-unknown-elf-gcc libfdt/libfdt.a 

run: opensbi libfdt kern
	$(MAKE) -C kernel run

clean:
	$(MAKE) -C kernel clean
	$(MAKE) -C opensbi clean
	$(MAKE) -C dtc clean
