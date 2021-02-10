# To build everything, run `make build`
# To clean all object files and the sysroot.img, run `make clean`
# To build only libc/kernel/user/etc run `make <project e.g. libc> && make sysroot` - note: some projects are dependencies for others, e.g. user/windowserver, for user. In this case, if user/windowserver is built, user should also be rebuilt
# To run the kernel in qemu, run `./cmds/run.sh`
# Examples:
#	Rebuild everything, run, then clean: `make build && ./cmds/run.sh && make clean`
# 	Rebuild the kernel, run, then clean: `make kernel && ./cmds/run.sh && make clean`
# 	Rebuild the windowsever: `make user/windowserver && make user`

MAKE=make
HOST=$(shell ./cmds/default-host.sh)
 
export AR=$(HOST)-ar
export AS=$(HOST)-as
 
export PREFIX=/usr
export BIN_PREFIX=/bin
export EXEC_PREFIX=$(PREFIX)
export BOOTDIR=/boot
export LIBDIR=$(EXEC_PREFIX)/lib
export INCLUDEDIR=$(PREFIX)/include
 
export CFLAGS=-O2 -g
export CPPFLAGS=
 
export SYSROOT="$(shell pwd)/src/sysroot"
export CC=$(HOST)-gcc --sysroot=$(SYSROOT) -isystem=$(INCLUDEDIR)

export LOCAL_CC=/usr/local/bin/gcc

build: headers libc kernel user/windowserver user sysroot

clean: clean-libc clean-kernel clean-user/windowserver clean-user clean-sysroot

headers:
	cd src/kernel && DESTDIR=$(SYSROOT) $(MAKE) install-headers -s
	cd src/libc && DESTDIR=$(SYSROOT) $(MAKE) install-headers -s
	cd src/user/windowserver && DESTDIR=$(SYSROOT) $(MAKE) install-headers -s

# dependency for everything
libc: headers
	cd src/libc && DESTDIR=$(SYSROOT) $(MAKE) install

clean-libc:
	cd src/libc && $(MAKE) clean -s

kernel: headers
	cd src/kernel && DESTDIR=$(SYSROOT) $(MAKE) install

clean-kernel:
	cd src/kernel && $(MAKE) clean -s

# dependency for user
user/windowserver: headers
	cd src/user/windowserver && DESTDIR=$(SYSROOT) $(MAKE) install

clean-user/windowserver:
	cd src/user/windowserver && $(MAKE) clean -s

user: headers
	cd src/user && DESTDIR=$(SYSROOT) $(MAKE) install

clean-user:
	cd src/user && $(MAKE) clean -s

sysroot:
	cp -r src/static/* src/sysroot
	python3 cmds/buildfs.py

clean-sysroot:
	rm -rf src/sysroot.img