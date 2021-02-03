import tarfile
import os

if os.path.exists("sysroot.img"):
	os.unlink("sysroot.img")

tar = tarfile.open("sysroot.img", "x:")
tar.add("sysroot/", "/", True)

tar.close()