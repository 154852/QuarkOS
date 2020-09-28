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
INCLUDEDIR?=$(PREFIX)/include
BINDIR?=$(EXEC_PREFIX)/bin

CFLAGS:=$(CFLAGS) -ffreestanding -Wall -Wextra -mgeneral-regs-only -static
CPPFLAGS:=$(CPPFLAGS)
LDFLAGS:=$(LDFLAGS)
LIBS:=$(LIBS) -nostdlib -lc -lgcc

CFLAGS:=$(CFLAGS) $(KERNEL_ARCH_CFLAGS)
CPPFLAGS:=$(CPPFLAGS) $(KERNEL_ARCH_CPPFLAGS)
LDFLAGS:=$(LDFLAGS) $(KERNEL_ARCH_LDFLAGS)
LIBS:=$(LIBS) $(KERNEL_ARCH_LIBS)

OBJS=\
hello.o

LINK_LIST=\
$(OBJS) \
$(LDFLAGS) \
$(LIBS) \

BINARIES=\
hello

.PHONY: all clean install install-headers install-executable
.SUFFIXES: .o .c

all: hello

hello: $(OBJS) link.ld
	$(CC) -T link.ld -o $@ $(CFLAGS) $(LINK_LIST)

.c.o:
	$(CC) -MD -c $< -o $@ -std=gnu11 $(CFLAGS) $(CPPFLAGS)

clean:
	rm -f $(OBJS) *.o */*.o */*/*.o
	rm -f $(OBJS:.o=.d) *.d */*.d */*/*.d
	rm -f $(BINARIES)

install: install-executable

install-headers:

install-executable: hello
	mkdir -p $(DESTDIR)$(BINDIR)
	cp $(BINARIES) $(DESTDIR)$(BINDIR)

-include $(OBJS:.o=.d)