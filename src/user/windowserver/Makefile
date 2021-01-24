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
LIBDIR?=$(EXEC_PREFIX)/lib
BINDIR?=$(EXEC_PREFIX)/bin

CFLAGS:=$(CFLAGS) -ffreestanding -Wall -Wextra -lc -nostartfiles
CPPFLAGS:=$(CPPFLAGS) -Iinclude -I/usr/include

CFLAGS:=$(CFLAGS) $(ARCH_CFLAGS)
CPPFLAGS:=$(CPPFLAGS) $(ARCH_CPPFLAGS)
 
SERVEROBJS=\
server/main.o

CLIENTOBJS=\
client/main.o
 
OBJS=\
$(SERVEROBJS) \
$(CLIENTOBJS)
 
EXECUTABLES=windowserver
LIBS=libwindowserver.a

.PHONY: all clean install install-headers install-libs
.SUFFIXES: .o .c .S
 
all: $(EXECUTABLES)
 
windowserver: $(SERVEROBJS)
	$(CC) $(SERVEROBJS) -o $@ $(CFLAGS) $(CPPFLAGS)

libwindowserver.a: $(CLIENTOBJS)
	$(AR) rcs $@ $(CLIENTOBJS)
 
.c.o:
	$(CC) -MD -c $< -o $@ -std=gnu11 $(CFLAGS) $(CPPFLAGS)
 
.S.o:
	$(CC) -MD -c $< -o $@ $(CFLAGS) $(CPPFLAGS)
 
clean:
	rm -f $(EXECUTABLES)
	rm -f $(LIBS)
	rm -f $(OBJS)
	rm -f $(OBJS:.o=.d)
 
install: install-headers install-libs install-exec
 
install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)/windowserver
	cp -R include/. $(DESTDIR)$(INCLUDEDIR)/windowserver

install-exec: $(EXECUTABLES)
	mkdir -p $(DESTDIR)$(BINDIR)
	cp $(EXECUTABLES) $(DESTDIR)$(BINDIR)

install-libs: $(LIBS)
	mkdir -p $(DESTDIR)$(LIBDIR)
	cp $(LIBS) $(DESTDIR)$(LIBDIR)
 
-include $(OBJS:.o=.d)
-include $(LIBK_OBJS:.o=.d)