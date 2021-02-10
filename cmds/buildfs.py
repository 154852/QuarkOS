import os

if not os.path.exists("src/sysroot/"):
	raise Exception("src/sysroot/ not found")

if os.path.exists("src/sysroot.img"):
	os.unlink("src/sysroot.img")

def get_size(start_path):
    total_size = 0
    for dirpath, dirnames, filenames in os.walk(start_path):
        for f in filenames:
            fp = os.path.join(dirpath, f)

            if not os.path.islink(fp):
                total_size += os.path.getsize(fp)

                print(f"name: {fp}")

    return total_size

size = 20000
cmd = f"genext2fs -b {size} -N 1000 -d src/sysroot/ src/sysroot.img"
os.system(cmd)
print(cmd)