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

build: headers libc kernel user/windowserver user sysroot
	
run:
	qemu-system-i386 -kernel src/sysroot/boot/quarkos.kernel -serial stdio -drive file=src/sysroot.img,format=raw -no-reboot -rtc base=utc,clock=host -vga std

clean:
	cd src/kernel && $(MAKE) clean -s
	cd src/libc && $(MAKE) clean -s
	cd src/user/windowserver && $(MAKE) clean -s
	cd src/user && $(MAKE) clean -s
	rm -rf src/sysroot

headers:
	cd src/kernel && DESTDIR=$(SYSROOT) $(MAKE) install-headers -s
	cd src/libc && DESTDIR=$(SYSROOT) $(MAKE) install-headers -s
	cd src/user/windowserver && DESTDIR=$(SYSROOT) $(MAKE) install-headers -s

libc:
	cd src/libc && DESTDIR=$(SYSROOT) $(MAKE) install

kernel:
	cd src/kernel && DESTDIR=$(SYSROOT) $(MAKE) install

user/windowserver:
	cd src/user/windowserver && DESTDIR=$(SYSROOT) $(MAKE) install

user:
	cd src/user && DESTDIR=$(SYSROOT) $(MAKE) install

sysroot:
	cp -r src/static/* src/sysroot
	python3 cmds/buildtar.py