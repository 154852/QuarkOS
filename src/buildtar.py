import tarfile
import os

if os.path.exists("sysroot.img"):
	os.unlink("sysroot.img")

tar = tarfile.open("sysroot.img", "x:")
tar.add("sysroot/", "/", True)

dev = tarfile.TarInfo(name="dev/")
dev.type = tarfile.DIRTYPE
tar.addfile(dev)

proc = tarfile.TarInfo(name="dev/proc/")
proc.type = tarfile.DIRTYPE
tar.addfile(proc)

tar.close()