import tarfile
import os

if not os.path.exists("src/sysroot/"):
	print("SYSROOT DOES NOT EXIST")
	exit(1)

if os.path.exists("src/sysroot.img"):
	os.unlink("src/sysroot.img")

tar = tarfile.open("src/sysroot.img", "x:")
tar.add("src/sysroot/", "/", True)

tar.close()
