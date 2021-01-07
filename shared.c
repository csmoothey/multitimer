#if LIN | APL
	#include <stddef.h>
#endif
#include "state.h"
#include "shared.h"
#include "volumeid.h"
#include "selectedid.h"
#include "modeid.h"
#include "XPLMPlugin.h"

#define SECONDS_IN_DAY	86400.0f

void changeClockTime(float delta) {
	clockTime += delta;
	if (clockTime >= SECONDS_IN_DAY) {
		clockTime -= SECONDS_IN_DAY;
	}
}

void initDevice() {
	zuluTime = XPLMGetDataf(drZuluTime);
	clockTime = XPLMGetDataf(drLocalTime);
	isSet = false;
	volume = VOL_ID_HIGH;
	selected = SEL_ID_CLOCK;
	for (int i = 0; i < 3; i++) {
		timers[i].isRunning = false;
		timers[i].mode = MODE_ID_UP;
		timers[i].time = 0.0;
		timers[i].recall = 0.0;
	}
}


void destroyWindow() {
	XPLMDestroyWindow(windowId);
	windowId = NULL;
}

void getPluginDir(char * buf) {
	char dirchar = *XPLMGetDirectorySeparator();
	XPLMGetPluginInfo(XPLMGetMyID(), NULL, buf, NULL, NULL);
	char * p = buf;
	char * slash = p;
	while (*p)
	{
		if (*p == dirchar) slash = p;
		p++;
	}
	slash++;
	*slash = 0;
}
