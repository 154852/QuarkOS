DEFAULT_HOST!=../default-host.sh
HOST?=DEFAULT_HOST
HOSTARCH!=../target-triplet-to-arch.sh $(HOST)
 
CFLAGS?=-O2 -g
CPPFLAGS?=
LDFLAGS?=
LIBS?=
 
DESTDIR?=
PREFIX?=/usr/local
EXEC_PREFIX?=$(PREFIX)
BOOTDIR?=$(EXEC_PREFIX)/boot
INCLUDEDIR?=$(PREFIX)/include
 
CFLAGS:=$(CFLAGS) -ffreestanding -Wall -Wextra -mgeneral-regs-only
CPPFLAGS:=$(CPPFLAGS) -D__is_kernel -Iinclude
LDFLAGS:=$(LDFLAGS)
LIBS:=$(LIBS) -nostdlib -lk -lgcc
 
# ARCHDIR=arch/$(HOSTARCH)
ARCHDIR=arch/i386
 
include $(ARCHDIR)/make.config
 
CFLAGS:=$(CFLAGS) $(KERNEL_ARCH_CFLAGS)
CPPFLAGS:=$(CPPFLAGS) $(KERNEL_ARCH_CPPFLAGS)
LDFLAGS:=$(LDFLAGS) $(KERNEL_ARCH_LDFLAGS)
LIBS:=$(LIBS) $(KERNEL_ARCH_LIBS)
 
KERNEL_OBJS=\
$(ARCHDIR)/boot.o \
kernel/hardware/gdt.o \
kernel/hardware/interrupts.o \
kernel/kernel.o \
kernel/hardware/keyboard.o \
kernel/hardware/keyboard_scancodes.o \
kernel/hardware/cmos.o \
kernel/kmalloc.o \
kernel/paging.o \
kernel/multiprocess.o \
kernel/hardware/pic.o \
kernel/hardware/pit.o \
kernel/hardware/disk.o \
kernel/tty.o \
kernel/elf.o \
kernel/ustar.o \
kernel/hardware/pci.o \
kernel/hardware/BXVGA.o \
kernel/ipcmessaging.o \
kernel/socket.o \
kernel/hardware/mouse.o \
kernel/syscalls/read.o \
kernel/syscalls/write.o \
kernel/syscalls/open.o \
kernel/syscalls/exit.o \
kernel/syscalls/exec.o \
kernel/syscalls/find_proc_id.o \
kernel/syscalls/framebuffer_info.o \
kernel/syscalls/framebuffer_set_state.o \
kernel/syscalls/get_pid.o \
kernel/syscalls/list_procs.o \
kernel/syscalls/proc_info.o \
kernel/syscalls/read_ipc_message.o \
kernel/syscalls/send_ipc_message.o \
kernel/syscalls/yield.o \
kernel/syscalls/mmap.o \
kernel/syscalls/readdir.o \
 
OBJS=\
$(ARCHDIR)/crti.o \
$(ARCHDIR)/crtbegin.o \
$(KERNEL_OBJS) \
$(ARCHDIR)/crtend.o \
$(ARCHDIR)/crtn.o \
 
LINK_LIST=\
$(LDFLAGS) \
$(ARCHDIR)/crti.o \
$(ARCHDIR)/crtbegin.o \
$(KERNEL_OBJS) \
$(LIBS) \
$(ARCHDIR)/crtend.o \
$(ARCHDIR)/crtn.o \
 
.PHONY: all clean install install-headers install-kernel
.SUFFIXES: .o .c .S .cpp
 
all: quarkos.kernel
 
quarkos.kernel: $(OBJS) $(ARCHDIR)/linker.ld
	$(CC) -T $(ARCHDIR)/linker.ld -o $@ $(CFLAGS) $(LINK_LIST)
	# grub-file --is-x86-multiboot quarkos.kernel
 
$(ARCHDIR)/crtbegin.o $(ARCHDIR)/crtend.o:
	OBJ=`$(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=$(@F)` && cp "$$OBJ" $@
 
.c.o:
	$(CC) -MD -c $< -o $@ -std=gnu11 $(CFLAGS) $(CPPFLAGS)

.cpp.o:
	$(CC) -MD -c $< -o $@ -std=c++11 $(CFLAGS) $(CPPFLAGS)
 
.S.o:
	$(CC) -MD -c $< -o $@ $(CFLAGS) $(CPPFLAGS)
 
clean:
	rm -f quarkos.kernel
	rm -f $(OBJS) *.o */*.o */*/*.o
	rm -f $(OBJS:.o=.d) *.d */*.d */*/*.d
 
install: install-headers install-kernel
 
install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -R include/. $(DESTDIR)$(INCLUDEDIR)/.
	#-p timestamps 
 
install-kernel: quarkos.kernel
	mkdir -p $(DESTDIR)$(BOOTDIR)
	cp quarkos.kernel $(DESTDIR)$(BOOTDIR)
 
-include $(OBJS:.o=.d)