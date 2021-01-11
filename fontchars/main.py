import os
from PIL import Image

out = """\
#ifndef _WINDOWSERVER_FONTCHARS_H
#define _WINDOWSERVER_FONTCHARS_H

{}
#endif\
"""

values = []

h2 = lambda x: ("0" + hex(x)[2:])[-2:]

names = [name for name in os.listdir(".") if name.endswith(".bmp")]
for name in names:
	image = Image.open(name).convert("RGBA")

	pixels = image.load()
	pixels_str = ""
	for y in range(image.height):
		for x in range(image.width):
			r, g, b, a = pixels[x, y]
			pixels_str += f"\\x{h2(r)}\\x{h2(g)}\\x{h2(b)}\\x{h2(a)}"

	char = name.split('.')[0]
	values.append(f"#define FONTCHAR_{char} \"{pixels_str}\"")
	values.append(f"#define FONTCHAR_{char}_W {image.width}")
	values.append(f"#define FONTCHAR_{char}_H {image.height}")
	values.append("")

out = out.format("\n".join(values))

with open("../src/builtfs/fontchars.h", "w") as f: f.write(out)
