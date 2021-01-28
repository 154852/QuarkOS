#include <windowserver/config.h>

#if ACTIVE_THEME_HACKER
	#include "color-hacker.h"
#elif ACTIVE_THEME_MACOS
	#include "color-macos.h"
#else
	#include "color-gray.h"
#endif