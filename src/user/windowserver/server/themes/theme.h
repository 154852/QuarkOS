#include "../window.h"

#ifndef _WINDOWSERVER_THEME_H
#define _WINDOWSERVER_THEME_H

typedef struct {
	void (*render_desktop_background)();
	void (*render_window)(InternalWindow* window);
	void (*render_cursor)();
} Theme;

Theme create_gray_theme();
Theme create_hacker_theme();
Theme create_macos_theme();

#endif