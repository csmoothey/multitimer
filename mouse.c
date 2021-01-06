#if LIN | APL
	#include <stddef.h>
#endif
#include "XPLMDisplay.h"
#include "mouse.h"
#include "shared.h"
#include "state.h"
#include "buttonid.h"
#include "selectedid.h"
#include "volumeid.h"
#include "cursorid.h"
#include "modeid.h"
#include "math.h"

#define SECONDS_IN_HOUR		3600.0f
#define SECONDS_IN_MINUTE	60.0f
#define ONE_SECOND			1.0f

static void handleHmsButton(float delta) {
	if (isSet) {
		changeClockTime(delta);
	}
	if (selected != SEL_ID_CLOCK) {
		int i = selected - 1;
		if (!timers[i].isRunning) {
			timers[i].time = floorf(timers[i].time + delta);
			timers[i].recall = timers[i].time;
			timers[i].mode = MODE_ID_DOWN;
		}
	}
}

static int testTopRowButtons(float ratio_X) {
	if (ratio_X >= RATIO_BTN_CLOCK_LEFT && ratio_X <= RATIO_BTN_CLOCK_RIGHT) {
		ftime(&clockPressedAt);
		isSet = false;
		buttonPressed = BTN_ID_CLOCK;
		selected = SEL_ID_CLOCK;
		return 1;
	}
	if (ratio_X >= RATIO_BTN_TIMER1_LEFT && ratio_X <= RATIO_BTN_TIMER1_RIGHT) {
		isSet = false;
		buttonPressed = BTN_ID_TIMER1;
		selected = SEL_ID_TIMER1;
		return 1;
	}
	if (ratio_X >= RATIO_BTN_TIMER2_LEFT && ratio_X <= RATIO_BTN_TIMER2_RIGHT) {
		isSet = false;
		buttonPressed = BTN_ID_TIMER2;
		selected = SEL_ID_TIMER2;
		return 1;
	}
	if (ratio_X >= RATIO_BTN_TIMER3_LEFT && ratio_X <= RATIO_BTN_TIMER3_RIGHT) {
		isSet = false;
		buttonPressed = BTN_ID_TIMER3;
		selected = SEL_ID_TIMER3;
		return 1;
	}
	return 1;
}

static int testBottomRowButtons(float ratio_X) {
	if (ratio_X >= RATIO_BTN_HR_LEFT && ratio_X <= RATIO_BTN_HR_RIGHT) {
		buttonPressed = BTN_ID_HR;
		handleHmsButton(SECONDS_IN_HOUR);
		return 1;
	}
	if (ratio_X >= RATIO_BTN_MIN_LEFT && ratio_X <= RATIO_BTN_MIN_RIGHT) {
		buttonPressed = BTN_ID_MIN;
		handleHmsButton(SECONDS_IN_MINUTE);
		return 1;
	}
	if (ratio_X >= RATIO_BTN_SEC_LEFT && ratio_X <= RATIO_BTN_SEC_RIGHT) {
		buttonPressed = BTN_ID_SEC;
		handleHmsButton(ONE_SECOND);
		return 1;
	}
	if (ratio_X >= RATIO_BTN_CLR_LEFT && ratio_X <= RATIO_BTN_CLR_RIGHT) {
		buttonPressed = BTN_ID_CLR;
		if (selected != SEL_ID_CLOCK) {
			int i = selected - 1;
			timers[i].time = 0.0;
			timers[i].mode = MODE_ID_UP;
		}
		return 1;
	}
	if (ratio_X >= RATIO_BTN_RCL_LEFT && ratio_X <= RATIO_BTN_RCL_RIGHT) {
		buttonPressed = BTN_ID_RCL;
		int i = selected - 1;
		if (selected != SEL_ID_CLOCK && timers[i].recall > 0) {
			timers[i].time = timers[i].recall;
			timers[i].mode = MODE_ID_DOWN;
		}
		return 1;
	}
	return 1;
}

static int testLeftColumnButtons(float ratio_Y) {
	if (ratio_Y <= RATIO_BTN_PWR_BOTTOM && ratio_Y >= RATIO_BTN_PWR_TOP) {
		buttonPressed = BTN_ID_PWR;
		isPwrOn = !isPwrOn;
		if (isPwrOn) {
			initDevice();
		}
		return 1;
	}
	if (isPwrOn && ratio_Y <= RATIO_BTN_VOL_BOTTOM && ratio_Y >= RATIO_BTN_VOL_TOP) {
		buttonPressed = BTN_ID_VOL;
		volume = (volume + 1) % 4;
		return 1;
	}
	return 1;
}

int	handleMouseClick(XPLMWindowID in_window_id, int x, int y, XPLMMouseStatus status, void * in_refcon)
{
	if (status == xplm_MouseUp) {
		buttonPressed = BTN_ID_NONE;
		isDrag = false;
		isResize = false;
		cursorType = CURSOR_ID_DEFAULT;
		handleCursor(in_window_id, x, y, NULL);
		return 1;
	}
	if (status == xplm_MouseDrag) {
		if (isDrag | isResize) {
			int deltaX = x - mouseX;
			int deltaY = y - mouseY;
			int l, t, r, b;
			XPLMGetWindowGeometry(windowId, &l, &t, &r, &b);
			if (isResize) {
				if (((r + deltaX) - l) < 154) {
					deltaX = 0;
				}
				if ((t - (b + deltaY)) < 100) {
					deltaY = 0;
				}
			}
			if (isDrag) {
				l += deltaX;
				t += deltaY;
			}
			r += deltaX;
			b += deltaY;
			XPLMSetWindowGeometry(windowId, l, t, r, b);
			baseDrawCoords.left = (float)l;
			baseDrawCoords.top = (float)t;
			baseDrawCoords.right = (float)r;
			baseDrawCoords.bottom = (float)b;
			mouseX = x;
			mouseY = y;
		}
		return 1;
	}
	float width = baseDrawCoords.right - baseDrawCoords.left + 1;
	float height = baseDrawCoords.top - baseDrawCoords.bottom + 1;

	float ratio_X = ((float)x - baseDrawCoords.left) / width;
	float ratio_Y = (baseDrawCoords.top - (float)y) / height;

	if (ratio_Y < RATIO_BTN_CLOCK_TOP) {
		mouseX = x;
		mouseY = y;
		isDrag = true;
		cursorType = CURSOR_ID_HANDDRAG;
		return 1;
	}
	if (ratio_Y > RATIO_BTN_STS_BOTTOM && ratio_X > RATIO_BTN_RCL_RIGHT) {
		mouseX = x;
		mouseY = y;
		isResize = true;
		cursorType = CURSOR_ID_HANDRESIZE;
		return 1;
	}

	if (isDrawClose && ratio_X >= RATIO_ICO_CLOSE_LEFT && ratio_X <= RATIO_ICO_CLOSE_RIGHT && ratio_Y <= RATIO_ICO_CLOSE_BOTTOM && ratio_Y >= RATIO_ICO_CLOSE_TOP) {
		buttonPressed = BTN_ID_CLOSE;
		isWindowVisible = false;
		destroyWindow();
		return 1;
	}

	if (isPwrOn) {
		if (ratio_Y <= RATIO_BTN_CLOCK_BOTTOM && ratio_Y >= RATIO_BTN_CLOCK_TOP) {
			return testTopRowButtons(ratio_X);
		}
		if (ratio_Y <= RATIO_BTN_HR_BOTTOM && ratio_Y >= RATIO_BTN_HR_TOP) {
			return testBottomRowButtons(ratio_X);
		}
		if (ratio_X >= RATIO_BTN_STS_LEFT && ratio_X <= RATIO_BTN_STS_RIGHT && ratio_Y <= RATIO_BTN_STS_BOTTOM && ratio_Y >= RATIO_BTN_STS_TOP) {
			buttonPressed = BTN_ID_STS;
			if (selected != SEL_ID_CLOCK) {
				int i = selected - 1;
				timers[i].isRunning = !timers[i].isRunning;
				if (timers[i].isRunning && timers[i].mode == MODE_ID_UP_AFTER_DOWN) {
					timers[i].mode = MODE_ID_UP;
				}
			}
			return 1;
		}
	}
	if (ratio_X >= RATIO_BTN_PWR_LEFT && ratio_X <= RATIO_BTN_PWR_RIGHT) {
		return testLeftColumnButtons(ratio_Y);
	}

	return 1;
}

XPLMCursorStatus handleCursor(XPLMWindowID window_id, int x, int y, void * in_refcon) {
	float width = baseDrawCoords.right - baseDrawCoords.left + 1.0f;
	float height = baseDrawCoords.top - baseDrawCoords.bottom + 1.0f;

	float ratio_X = ((float)x - baseDrawCoords.left) / width;
	float ratio_Y = (baseDrawCoords.top - (float)y) / height;

	isDrawClose = (ratio_X < RATIO_BTN_CLOCK_LEFT && ratio_Y < RATIO_BTN_PWR_TOP) ? true : false;
	if (isDrag || isResize) {
		return xplm_CursorHidden;
	}
	if (ratio_Y < RATIO_BTN_CLOCK_TOP || (ratio_Y > RATIO_BTN_STS_BOTTOM && ratio_X > RATIO_BTN_RCL_RIGHT)) {
		cursorType = CURSOR_ID_HAND;
		return xplm_CursorHidden;
	}
	cursorType = CURSOR_ID_DEFAULT;
	return xplm_CursorDefault;
}
