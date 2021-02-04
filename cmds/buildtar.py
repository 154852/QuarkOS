import tarfile
import os

if os.path.exists("src/sysroot.img"):
	os.unlink("src/sysroot.img")

tar = tarfile.open("src/sysroot.img", "x:")
tar.add("src/sysroot/", "/", True)

tar.close()
