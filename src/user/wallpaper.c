#include "windowserver/color.h"
#include "windowserver/fontchars.h"
#include "windowserver/themes/color-active.h"
#include "windowserver/config.h"
#include <string.h>
#include <syscall.h>
#include <stdio.h>
#include <assertions.h>
#include <windowserver/wsmsg.h>
#include <windowserver/client.h>
#include <windowserver/mainloop.h>
#include <ckeyboard.h>

WindowHandle windowhandle;

int main() {
	windowhandle = create_window_detailed("Wallpaper", SUPPORTED_WIDTH, SUPPORTED_HEIGHT, 0, 0, 0, pixel_from_rgba(243, 22, 145, 0xff));

	ImageLoadResponse image = load_image(windowhandle, "system/wallpaper.bmp");
	create_image(windowhandle, 0, 0, image.width, image.height, image.id);

	render_window(windowhandle);
	while (1) {
		yield();
	}

	return 0;
}
