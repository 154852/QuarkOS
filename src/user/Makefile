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

CFLAGS:=$(CFLAGS) -ffreestanding -Wall -Wextra -static
CPPFLAGS:=$(CPPFLAGS)
LDFLAGS:=$(LDFLAGS)
LIBS:=$(LIBS) -lc -lgcc

CFLAGS:=$(CFLAGS) $(KERNEL_ARCH_CFLAGS) -nostartfiles
CPPFLAGS:=$(CPPFLAGS) $(KERNEL_ARCH_CPPFLAGS)
LDFLAGS:=$(LDFLAGS) $(KERNEL_ARCH_LDFLAGS)
LIBS:=$(LIBS) $(KERNEL_ARCH_LIBS)

LINK_LIST=\
$(LDFLAGS) \
$(LIBS) \

OBJS=\
hello.o \
shell.o \
ps.o \
guiapp.o \
dock.o \
calc.o \
font.o \
tasks.o

BINARIES=$(OBJS:.o=)

.PHONY: all clean install install-headers install-executable
.SUFFIXES: .o .c

all: $(BINARIES)

hello: hello.o
	$(CC) -o $@ $(CFLAGS) $< $(LINK_LIST)

shell: shell.o
	$(CC) -o $@ $(CFLAGS) $< $(LINK_LIST)

ps: ps.o
	$(CC) -o $@ $(CFLAGS) $< $(LINK_LIST)

guiapp: guiapp.o
	$(CC) -o $@ $(CFLAGS) $< $(LINK_LIST) -lwindowserver

dock: dock.o
	$(CC) -o $@ $(CFLAGS) $< $(LINK_LIST) -lwindowserver

calc: calc.o
	$(CC) -o $@ $(CFLAGS) $< $(LINK_LIST) -lwindowserver

font: font.o
	$(CC) -o $@ $(CFLAGS) $< $(LINK_LIST) -lwindowserver

tasks: tasks.o
	$(CC) -o $@ $(CFLAGS) $< $(LINK_LIST) -lwindowserver

.c.o:
	$(CC) -MD -c $< -o $@ -std=gnu11 $(CFLAGS) $(CPPFLAGS)

.S.o:
	$(CC) -o $@ -c $< -lc -static

clean:
	rm -f $(OBJS) *.o */*.o */*/*.o
	rm -f $(OBJS:.o=.d) *.d */*.d */*/*.d
	rm -f $(BINARIES)

install: install-executable

install-headers:

install-executable: $(BINARIES)
	mkdir -p $(DESTDIR)$(BINDIR)
	cp $(BINARIES) $(DESTDIR)$(BINDIR)

-include $(OBJS:.o=.d)