KDIR := /usr/src/linux
obj-m += meminfo_netlink.o
obj-k := meminfo_netlink.ko

all:
	$(MAKE) LLVM=1 LLVM_IAS=1 -C $(KDIR) M=$(shell pwd) modules
	$(KDIR)/scripts/sign-file sha3-512 $(KDIR)/certs/signing_key.pem $(KDIR)/certs/signing_key.x509 $(obj-k)

clean:
	$(MAKE) LLVM=1 LLVM_IAS=1 -C /usr/src/linux M=$(shell pwd) -j8 clean

test:
	insmod $(obj-k)

rmmod:
	rmmod $(obj-k)

install:
	$(MAKE) LLVM=1 LLVM_IAS=1 -C $(KDIR) M=$(shell pwd) modules_install
