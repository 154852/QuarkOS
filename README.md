# QuarkOS

A simple 32 bit, graphical, OS written in C++.


## Building
1. Install python3, make
2. You will need to build a i686-elf-gcc cross compiler. There are not fixed requirements for it, so long as the commands `i686-elf-gcc`, `i686-elf-ar`, `qemu-system-i386` are in the `PATH`
See [here](https://wiki.osdev.org/GCC_Cross-Compiler) for building 
3. `cd src`
4. Run `./build.sh` to build, `./qemu.sh` to build and run, `./start.sh` to run or `./build.sh` to build

## Note
The system is by no means well tested, it works on my machine and that's about as much as I can guarantee :)

## Structure
`src` - The root of all source code
	`kernel`
		`arch/i386` - Mostly boot code for the kernel.
		`include/kernel` - Mapped into `sysroot/usr/include/kernel` on build
		`kernel` - Kernel C++ source
			`hardware` - Code relating specifically to interacting with hardware devices, whether that is the CPU, keyboard, mouse, monitor or something lower level
	`libc`
		`arch/i386` - At the time of writing contains only the crt0
		`include` - Mapped into `sysroot/usr/include` on build
	`user` - All userspace code goes in here
		`windowserver`
			`server` - The server process
			`client` - The client windowserver library
			`utils` - Code accessible by both the server and the client
		`calc.c` - A simple GUI calculator
		`dock.c` - An application started by the windowserver which opens other apps
		`font.c` - Font viewer
		`guiapp.c` - A demonstration of windowserver code
		`hello.c` - Probably won't work any more, was made for the command line interface which no longer exists
		`ps.c` - Lists all running processes
		`shell.c` - A shell for the command line interface
	`buildtar.py` - Creates the .tar file which acts as the file system (for now)
