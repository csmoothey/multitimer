#pragma once

#if IBM
	#include <sys\timeb.h>
#else
	#include <sys/timeb.h>
#endif
#include <stdbool.h>
#include "metrics.h"
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMDataAccess.h"
#include "XPLMProcessing.h"
#include "XPLMMenus.h"
#include "types.h"

extern XPLMWindowID	windowId;
extern XPLMDataRef drZuluTime;
extern XPLMDataRef drLocalTime;
extern XPLMDataRef drIsInReplay;
extern coords baseDrawCoords;
extern byte buttonPressed;
extern boolean isPwrOn;
extern byte selected;
extern byte volume;
extern float clockTime;
extern float zuluTime;
extern boolean isSet;
extern struct timeb clockPressedAt;
extern timer timers[3];
extern XPLMFlightLoopID timerFloopId;
extern XPLMFlightLoopID mouseFloopId;
extern XPLMCommandRef commandId;
extern boolean isWindowVisible;
extern boolean isDrag;
extern int mouseX, mouseY;
extern boolean isDrawClose;
extern byte cursorType;
extern boolean isResize;
extern XPLMMenuID g_menu_id;
