# QuarkOS

A simple 32 bit graphical OS written in C++.


## Building
1. Install python3, make
2. You will need to build a i686-elf-gcc cross compiler. There are not fixed requirements for it, so long as the commands `i686-elf-gcc`, `i686-elf-ar`, `qemu-system-i386` are in the `PATH`
See [this page](https://wiki.osdev.org/GCC_Cross-Compiler) on how to do that.
3. `cd src`
4. Run `./build.sh` to build, `./qemu.sh` to build and run, `./start.sh` to run or `./build.sh` to build

## Note
The system is by no means well tested, it works on my machine and that's about as much as I can guarantee :)

## Structure
```
src - The root of all source code
 kernel
  arch/i386 - Mostly boot code for the kernel.
  include/kernel - Mapped into sysroot/usr/include/kernel on build
  kernel - Kernel C++ source
   hardware - Code relating specifically to interacting with hardware devices, whether that is the CPU, keyboard, mouse, monitor or something lower level
 libc
  arch/i386 - At the time of writing contains only the crt0
  include - Mapped into sysroot/usr/include on build
 user - All userspace code goes in here
  windowserver
   server - The server process
   client - The client windowserver library
   utils - Code accessible by both the server and the client
  calc.c - A simple GUI calculator
  dock.c - An application started by the windowserver which opens other apps
  font.c - Font viewer
  guiapp.c - A demonstration of windowserver code
  hello.c - Probably won't work any more, was made for the command line interface which no longer exists
  ps.c - Lists all running processes
  shell.c - A shell for the command line interface
 buildtar.py - Creates the .tar file which acts as the file system (for now)
```

## Code Walkthrough
1. After building, you should see a folder (in the src directory) called sysroot, and a file named sysroot.img. Sysroot is effectively the root filesystem - it was populated by various programs, headers and any other files by the build process (`make install*`). [buildtar.py](/src/buildtar.py) then created a [tar](https://wiki.osdev.org/USTAR) file of this (and removes the `sysroot` root directory name).
2. QEMU will start the kernel in sysroot/boot/quarkos.kernel
3. The very first line of kernel code is in [kernel/arch/i386/boot.S](/src/kernel/arch/i386/boot.S), which pretty much immediately calls `kernel_main`, defined in [kernel/kernel/kernel.cpp](/src/kernel/kernel/kernel.cpp). This is the kernel entry point.
4. Ignoring logging, the first things we do are set up interrupts.
	1. First we [PIC](https://wiki.osdev.org/PIC)::remap in [kernel/kernel/hardware/pic.cpp](/src/kernel/kernel/hardware/pic.cpp), pushing hardware interrupts into the range 0x20-0x30
	2. We then create the [GDT](https://wiki.osdev.org/GDT)
	3. Now we can create the interrupt handlers, most of them are defaults, with a few exceptions for specfic actions, which we send to be loaded into the CPU [kernel/kernel/hardware/interrupts.cpp](/src/kernel/kernel/hardware/interrupts.cpp)
	4. After this we can create the syscall_table, where the syscall implementations can be seen in [kernel/kernel/syscalls](/src/kernel/kernel/syscalls.hpp)
5. Moving onto the initialisation of other things:
	1. [PCI](https://wiki.osdev.org/PCI) is a means of communication between hardware devices and the OS - [kernel/kernel/hardware/pci.cpp](/src/kernel/kernel/hardware/pci.cpp)
	2. [BXVGA](https://wiki.osdev.org/BGA) will provide the monitor support - [kernel/kernel/hardware/BXVGA.cpp](/src/kernel/kernel/hardware/BXVGA.cpp)
	3. The disk - [kernel/kernel/hardware/disk.cpp](/src/kernel/kernel/hardware/disk.cpp) (although the filesystem itself is handled by [kernel/kernel/ustar.cpp](/src/kernel/kernel/ustar.cpp))
	4. The PS2 [mouse](/src/kernel/kernel/hardware/mouse.cpp) and [keyboard](/src/kernel/kernel/hardware/keyboard.cpp) (though the keyboard doesn't really need to do anything) 
6. Initialise memory [paging](https://wiki.osdev.org/Paging) - [kernel/kernel/paging.cpp](/src/kernel/kernel/paging.cpp)
7. Set up Multiprocessing - [kernel/kernel/multiprocess.cpp](/src/kernel/kernel/multiprocess.cpp)
8. Setup the terminal (only exists until the windowserver process has started, and not even before in this case) - [kernel/kernel/tty.cpp](/src/kernel/kernel/tty.cpp)
9. We can finally read the windowserver from disk, decode it as an elf process, setup it's memory and schedule it as the first process
10. Now we start the [PIT](https://wiki.osdev.org/PIT), and the kernel has finished it's initialisation