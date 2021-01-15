import os
from PIL import Image

out = """\
#ifndef _WINDOWSERVER_FONTCHARS_H
#define _WINDOWSERVER_FONTCHARS_H

typedef struct {{
	int width;
	int height;
	char* raw;
}} FontChar;

{}
FontChar fontchar_for_char(char chr) {{
	switch (chr) {{
	case ' ': return (FontChar) {{ .width=FONTCHAR_A_W, .height=0, .raw=0 }};
{}
	default: return (FontChar) {{ .width=0, .height=0, .raw=0 }};
	}}
}}

#endif\
"""

values = []
switch_values = []

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
	values.append(f"#define FONTCHAR_{char}_RAW \"{pixels_str}\"")
	values.append(f"#define FONTCHAR_{char}_W {image.width}")
	values.append(f"#define FONTCHAR_{char}_H {image.height}")
	values.append("")

	switch_values.append(f"\tcase '{char}':\n\tcase '{char.lower()}':\n\t\treturn (FontChar) {{ .width=FONTCHAR_{char}_W, .height=FONTCHAR_{char}_H, .raw=FONTCHAR_{char}_RAW }};")

out = out.format("\n".join(values), "\n".join(switch_values))

with open("../src/builtfs/fontchars.h", "w") as f: f.write(out)
