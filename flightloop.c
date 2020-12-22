#include "flightloop.h"
#include "state.h"
#include "shared.h"
#include "buttonid.h"
#include "cursorid.h"
#include "modeid.h"
#include "soundcore.h"
#include "soundid.h"

#define SET_DELAY_MILLIS	2000
#define MILLIS_IN_SECOND	1000
#define SECONDS_IN_DAY		86400.0f

static void updateIsSet() {
	struct timeb now;
	ftime(&now);
	int diff = (MILLIS_IN_SECOND * (int)(now.time - clockPressedAt.time) + (now.millitm - clockPressedAt.millitm));
	if (diff > SET_DELAY_MILLIS) {
		isSet = true;
	}
}

static int getAudioTimerId() {
	int lowestId = -1;
	float lowestTime = 61.0f;
	for (int timerId = 0; timerId < 3; timerId++) {
		if (timers[timerId].mode == MODE_ID_DOWN && timers[timerId].isRunning && timers[timerId].time < lowestTime) {
			lowestId = timerId;
			lowestTime = timers[timerId].time;
		}
	}
	return lowestId;
}

static void speakIfNeeded(int timerId, float delta) {
	if (timers[timerId].time > 60.0f && (timers[timerId].time - delta) <= 60.0f) {
		playSound(SOUND_ID_60_SECONDS, volume);
		return;
	}
	if (timers[timerId].time > 30.0f && (timers[timerId].time - delta) <= 30.0f) {
		playSound(SOUND_ID_30_SECONDS, volume);
		return;
	}
	if (timers[timerId].time > 15.0f && (timers[timerId].time - delta) <= 15.0f) {
		playSound(SOUND_ID_15_SECONDS, volume);
		return;
	}
	if (timers[timerId].time > 5.0f && (timers[timerId].time - delta) <= 5.0f) {
		playSound(SOUND_ID_5_SECONDS, volume);
		return;
	}
	if (timers[timerId].time > 0.0f && (timers[timerId].time - delta) <= 0.0f) {
		playSound(SOUND_ID_MARK, volume);
	}
}

static void updateTime() {
	float newZuluTime = XPLMGetDataf(drZuluTime);
	float delta = newZuluTime - zuluTime;
	// Handle the flip from 23:59:59 to 00:00:00
	if (delta < 0) {
		delta = SECONDS_IN_DAY - zuluTime + newZuluTime;
	}
	zuluTime = newZuluTime;
	changeClockTime(delta);
	int audioTimerId = getAudioTimerId();
	for (int timerId = 0; timerId < 3; timerId++) {
		if (!timers[timerId].isRunning) {
			continue;
		}
		if (timers[timerId].mode != MODE_ID_DOWN) {
			timers[timerId].time += delta;
			continue;
		}
		if (timerId == audioTimerId) {
			speakIfNeeded(timerId, delta);
		}
		timers[timerId].time -= delta;
		if (timers[timerId].time < 0.0) {
			timers[timerId].time *= -1.0;
			timers[timerId].mode = MODE_ID_UP_AFTER_DOWN;
		}
	}
}

float timerFloopCb(float elapsed1, float elapsed2, int ctr, void* refcon) {
	if (XPLMGetDatai(drIsInReplay) == 0) {
		updateTime();
	}
	if (buttonPressed == BTN_ID_CLOCK && !isSet) {
		updateIsSet();
	}
	return 0.1f;
}

float mouseFloopCb(float elapsed1, float elapsed2, int ctr, void* refcon) {
	int x, y;
	XPLMGetMouseLocationGlobal(&x, &y);
	if (isDrawClose && x < baseDrawCoords.left || x > baseDrawCoords.right || y < baseDrawCoords.bottom || y > baseDrawCoords.top) {
		isDrawClose = false;
		if (cursorType == CURSOR_ID_HAND) {
			cursorType = CURSOR_ID_DEFAULT;
		}
	}
	return 0.25f;
}
