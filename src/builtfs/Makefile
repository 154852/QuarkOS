DEFAULT_HOST!=../default-host.sh
HOST?=DEFAULT_HOST
HOSTARCH!=../target-triplet-to-arch.sh $(HOST)
 
CFLAGS?=-O2 -g
CPPFLAGS?=
LDFLAGS?=
LIBS?=../sysroot/usr/lib/libk.a
 
DESTDIR?=
PREFIX?=#/usr/local
EXECDIR?=$(BIN_PREFIX)
 
CFLAGS:=$(CFLAGS) -ffreestanding -Wall -Wextra -mgeneral-regs-only
CPPFLAGS:=$(CPPFLAGS) -D__is_kernel -Iinclude
LDFLAGS:=$(LDFLAGS)
LIBS:=$(LIBS) -nostdlib -lk -lgcc
 
CFLAGS:=$(CFLAGS) $(KERNEL_ARCH_CFLAGS)
CPPFLAGS:=$(CPPFLAGS) $(KERNEL_ARCH_CPPFLAGS)
LDFLAGS:=$(LDFLAGS) $(KERNEL_ARCH_LDFLAGS)
LIBS:=$(LIBS) $(KERNEL_ARCH_LIBS)

OBJS=\
hello.o

LINK_LIST=\
$(LDFLAGS) \
$(LIBS) \
 
.PHONY: all clean install install-headers install-executable
.SUFFIXES: .o .c .S .cpp
 
all: hello
 
hello: $(OBJS) link.ld
	$(CC) -T link.ld -o $@ $(CFLAGS) $(LINK_LIST)
 
.c.o:
	$(CC) -MD -c $< -o $@ -std=gnu11 $(CFLAGS) $(CPPFLAGS)
 
clean:
	rm -f $(OBJS) *.o */*.o */*/*.o
	rm -f $(OBJS:.o=.d) *.d */*.d */*/*.d
 
install: install-executable
 
install-executable: hello
	mkdir -p $(DESTDIR)$(EXECDIR)
	cp hello $(DESTDIR)$(EXECDIR)
 
-include $(OBJS:.o=.d)