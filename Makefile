KDIR := /usr/src/linux
obj-m += meminfo_netlink.o
obj-k := meminfo_netlink.ko
EXTRA_CFLAGS := -I$(src)/include -Wall
test-o := meminfo_test

all:
	$(MAKE) LLVM=1 LLVM_IAS=1 -C $(KDIR) M=$(shell pwd) modules
	$(KDIR)/scripts/sign-file sha3-512 $(KDIR)/certs/signing_key.pem $(KDIR)/certs/signing_key.x509 $(obj-k)

clean:
	$(MAKE) LLVM=1 LLVM_IAS=1 -C /usr/src/linux M=$(shell pwd) -j8 clean
	rm -f meminfo_test
	rm -rf usr/

test: all
	insmod $(obj-k)
	$(MAKE) ./$(test-o).c
	./$(test-o)

rmmod:
	rmmod $(obj-k)

install:
	$(MAKE) LLVM=1 LLVM_IAS=1 -C $(KDIR) M=$(shell pwd) modules_install

headers:
	mkdir -p usr/include/
	cp -r include/uapi/* usr/include
	sed -i 's|_UAPI_\(.*\)|_\1|' usr/include/*/*.h

headers_install: headers
	cp -r usr /
