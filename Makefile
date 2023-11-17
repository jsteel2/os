all: opensbi kern

.PHONY: kern opensbi run clean

kern:
	$(MAKE) -C kernel

opensbi:
	$(MAKE) -C opensbi PLATFORM=generic CROSS_COMPILE=riscv64-linux-gnu-

run: kern opensbi
	$(MAKE) -C kernel run

clean:
	$(MAKE) -C kernel clean
	$(MAKE) -C opensbi clean
