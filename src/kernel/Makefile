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
CPPFLAGS:=$(CPPFLAGS) -D__is_kernel -Iinclude -Ifs/include
LDFLAGS:=$(LDFLAGS)
LIBS:=$(LIBS) -nostdlib -lk -lgcc
 
# ARCHDIR=arch/$(HOSTARCH)
ARCHDIR=arch/i386
 
include $(ARCHDIR)/make.config
 
CFLAGS:=$(CFLAGS) $(KERNEL_ARCH_CFLAGS)
CPPFLAGS:=$(CPPFLAGS) $(KERNEL_ARCH_CPPFLAGS)
LDFLAGS:=$(LDFLAGS) $(KERNEL_ARCH_LDFLAGS)
LIBS:=$(LIBS) $(KERNEL_ARCH_LIBS)

FS_OBJS=\
fs/ext2/init.o\
fs/ext2/path.o

FS_EXEC_OBJS=\
fs/ext2/main.local.o
 
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
kernel/syscalls/read_ipc_message.o \
kernel/syscalls/send_ipc_message.o \
kernel/syscalls/yield.o \
kernel/syscalls/mmap.o \
kernel/syscalls/readdir.o \
kernel/syscalls/gettime.o \
kernel/syscalls/getfulltime.o \
$(FS_OBJS)
 
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
 
.PHONY: all clean install install-headers install-kernel install-ext2fs
.SUFFIXES: .o .local.o .c .S .cpp
 
all: quarkos.kernel
 
quarkos.kernel: $(OBJS) $(ARCHDIR)/linker.ld
	$(CC) -T $(ARCHDIR)/linker.ld -o $@ $(CFLAGS) $(LINK_LIST)

ext2fs: $(FS_OBJS:.o=.local.o) $(FS_EXEC_OBJS)
	$(LOCAL_CC) $(FS_OBJS:.o=.local.o) $(FS_EXEC_OBJS) -o $@
 
$(ARCHDIR)/crtbegin.o $(ARCHDIR)/crtend.o:
	OBJ=`$(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=$(@F)` && cp "$$OBJ" $@
 
.c.o:
	$(CC) -MD -c $< -o $@ -std=gnu11 $(CFLAGS) $(CPPFLAGS)

.c.local.o:
	$(LOCAL_CC) -MD -c $< -o $@ -std=gnu11 -Iinclude -Ifs/include -D__is_local -D__is_kernel

.cpp.local.o:
	$(LOCAL_CC) -MD -c $< -o $@ -std=c++11 -Iinclude -Ifs/include -D__is_local -D__is_kernel

.cpp.o:
	$(CC) -MD -c $< -o $@ -std=c++11 $(CFLAGS) $(CPPFLAGS)

.S.o:
	$(CC) -MD -c $< -o $@ $(CFLAGS) $(CPPFLAGS)
 
clean:
	rm -f quarkos.kernel
	rm -f $(OBJS)
	rm -f $(OBJS:.o=.d)
	rm -f $(FS_OBJS:.o=.local.o)
	rm -f $(FS_OBJS:.o=.local.d)
	rm -f $(FS_EXEC_OBJS)
	rm -f $(FS_EXEC_OBJS:.o=.d)
	rm -f ext2fs
 
install: install-headers install-kernel #install-ext2fs
 
install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -R include/. $(DESTDIR)$(INCLUDEDIR)/.
	cp -R fs/include/. $(DESTDIR)$(INCLUDEDIR)/.
 
install-kernel: quarkos.kernel
	mkdir -p $(DESTDIR)$(BOOTDIR)
	cp quarkos.kernel $(DESTDIR)$(BOOTDIR)

install-ext2fs: ext2fs
 
-include $(OBJS:.o=.d)