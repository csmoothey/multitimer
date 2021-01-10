#include "drawcore.h"
#include <GL/glpng.h>
#include <GL/gl.h>
#include "drawbtnboundary.h"
#include "state.h"
#include "selectedid.h"
#include "volumeid.h"
#include "cursorid.h"
#include "modeid.h"
#include "math.h"

static XPLMTextureID textureId;

static void drawTexture(coords tc, coords dc) {
	float sqrt2 = 1.414213562f;
	float f = 1.0f/(float)pow(sqrt2, (double)brightness);
	glColor4f(f, f, f, 1.0);
	glBegin(GL_QUADS);
	glTexCoord2f(tc.left, tc.top); glVertex2f(dc.left, dc.bottom);
	glTexCoord2f(tc.left, tc.bottom); glVertex2f(dc.left, dc.top);
	glTexCoord2f(tc.right, tc.bottom); glVertex2f(dc.right, dc.top);
	glTexCoord2f(tc.right, tc.top); glVertex2f(dc.right, dc.bottom);
	glEnd();
}

static void drawBase(coords dc) {
	coords tc;

	tc.left = PIX_TIMER_LEFT / TEXTURE_WIDTH;
	tc.bottom = PIX_TIMER_TOP / TEXTURE_HEIGHT;
	tc.right = PIX_TIMER_RIGHT / TEXTURE_WIDTH;
	tc.top = PIX_TIMER_BOTTOM / TEXTURE_HEIGHT;

	drawTexture(tc, dc);
}

static void drawBattery(coords base) {
	coords tc, dc;

	tc.left = PIX_ICO_BAT_LEFT / TEXTURE_WIDTH;
	tc.bottom = PIX_ICO_BAT_TOP / TEXTURE_HEIGHT;
	tc.right = PIX_ICO_BAT_RIGHT / TEXTURE_WIDTH;
	tc.top = PIX_ICO_BAT_BOTTOM / TEXTURE_HEIGHT;

	float width = base.right - base.left + 1;
	float height = base.top - base.bottom + 1;

	dc.left = base.left + RATIO_ICO_BAT_LEFT * width;
	dc.top = base.top - RATIO_ICO_BAT_TOP * height;
	dc.right = base.left + RATIO_ICO_BAT_RIGHT * width;
	dc.bottom = base.top - RATIO_ICO_BAT_BOTTOM * height;

	drawTexture(tc, dc);
}

static void drawSelected(coords base, byte selected) {
	coords tc, dc;

	tc.left = PIX_ICO_BAR_LEFT / TEXTURE_WIDTH;
	tc.bottom = PIX_ICO_BAR_TOP / TEXTURE_HEIGHT;
	tc.right = PIX_ICO_BAR_RIGHT / TEXTURE_WIDTH;
	tc.top = PIX_ICO_BAR_BOTTOM / TEXTURE_HEIGHT;

	float width = base.right - base.left + 1;
	float height = base.top - base.bottom + 1;

	dc.bottom = base.top - RATIO_ICO_BAR_BOTTOM * height;
	dc.top = base.top - RATIO_ICO_BAR_TOP * height;

	switch (selected)
	{
		case SEL_ID_CLOCK:
			dc.left = base.left + RATIO_ICO_BAR0_LEFT * width;
			dc.right = base.left + RATIO_ICO_BAR0_RIGHT * width;
			break;
		case SEL_ID_TIMER1:
			dc.left = base.left + RATIO_ICO_BAR1_LEFT * width;
			dc.right = base.left + RATIO_ICO_BAR1_RIGHT * width;
			break;
		case SEL_ID_TIMER2:
			dc.left = base.left + RATIO_ICO_BAR2_LEFT * width;
			dc.right = base.left + RATIO_ICO_BAR2_RIGHT * width;
			break;
		case SEL_ID_TIMER3:
			dc.left = base.left + RATIO_ICO_BAR3_LEFT * width;
			dc.right = base.left + RATIO_ICO_BAR3_RIGHT * width;
			break;
		default:
			return;
	}

	drawTexture(tc, dc);
}

static void drawVolume(coords base, byte volume) {
	coords tc, dc;

	tc.bottom = PIX_ICO_VOL_TOP / TEXTURE_HEIGHT;
	tc.top = PIX_ICO_VOL_BOTTOM / TEXTURE_HEIGHT;

	float width = base.right - base.left + 1;
	float height = base.top - base.bottom + 1;

	dc.bottom = base.top - RATIO_ICO_VOL_BOTTOM * height;
	dc.top = base.top - RATIO_ICO_VOL_TOP * height;
	dc.left = base.left + RATIO_ICO_VOL_LEFT * width;

	switch (volume)
	{
		case VOL_ID_OFF:
			tc.left = PIX_ICO_VOL0_LEFT / TEXTURE_WIDTH;
			tc.right = PIX_ICO_VOL0_RIGHT / TEXTURE_WIDTH;
			dc.right = base.left + RATIO_ICO_VOL0_RIGHT * width;
			break;
		case VOL_ID_LOW:
			tc.left = PIX_ICO_VOL1_LEFT / TEXTURE_WIDTH;
			tc.right = PIX_ICO_VOL1_RIGHT / TEXTURE_WIDTH;
			dc.right = base.left + RATIO_ICO_VOL1_RIGHT * width;
			break;
		case VOL_ID_MED:
			tc.left = PIX_ICO_VOL2_LEFT / TEXTURE_WIDTH;
			tc.right = PIX_ICO_VOL2_RIGHT / TEXTURE_WIDTH;
			dc.right = base.left + RATIO_ICO_VOL2_RIGHT * width;
			break;
		case VOL_ID_HIGH:
			tc.left = PIX_ICO_VOL3_LEFT / TEXTURE_WIDTH;
			tc.right = PIX_ICO_VOL3_RIGHT / TEXTURE_WIDTH;
			dc.right = base.left + RATIO_ICO_VOL3_RIGHT * width;
			break;
		default:
			return;
	}

	drawTexture(tc, dc);
}

static coords textureCoordsNumber(byte number, boolean isBig) {
	coords tc;
	float left;
	float width;
	if (isBig) {
		left = PIX_ICO_BIG_DIGIT_LEFT;
		width = PIX_ICO_BIG_DIGIT_WIDTH;
		tc.top = PIX_ICO_BIG_DIGIT_BOTTOM / TEXTURE_HEIGHT;
		tc.bottom = PIX_ICO_BIG_DIGIT_TOP / TEXTURE_HEIGHT;
	} else {
		left = PIX_ICO_SMALL_DIGIT_LEFT;
		width = PIX_ICO_SMALL_DIGIT_WIDTH;
		tc.top = PIX_ICO_SMALL_DIGIT_BOTTOM / TEXTURE_HEIGHT;
		tc.bottom = PIX_ICO_SMALL_DIGIT_TOP / TEXTURE_HEIGHT;
	}
	tc.left = left + (float)number * width;
	tc.right = tc.left + width;
	tc.left /= TEXTURE_WIDTH;
	tc.right /= TEXTURE_WIDTH;
	return tc;
}

static void drawDigit(byte number, boolean isBig, coords base, float top, float left) {
	coords tc = textureCoordsNumber(number, isBig);
	coords dc;

	float width = base.right - base.left + 1;
	float height = base.top - base.bottom + 1;

	dc.left = base.left + left * width;
	dc.top = base.top - top * height;

	if (isBig) {
		dc.right = base.left + (left + RATIO_ICO_BIG_DIGIT_WIDTH) * width;
		dc.bottom = base.top - (top + RATIO_ICO_BIG_DIGIT_HEIGHT) * height;
	}
	else {
		dc.right = base.left + (left + RATIO_ICO_SMALL_DIGIT_WIDTH) * width;
		dc.bottom = base.top - (top + RATIO_ICO_SMALL_DIGIT_HEIGHT) * height;
	}

	drawTexture(tc, dc);
}

static void drawTime(byte hour, byte minute, byte second, coords base) {
	float left = RATIO_ICO_DISP_TIME_LEFT;
	drawDigit(hour / 10, true, base, RATIO_ICO_DISP_TIME_TOP, left);
	left += RATIO_ICO_BIG_DIGIT_WIDTH;
	drawDigit(hour % 10, true, base, RATIO_ICO_DISP_TIME_TOP, left);
	left += RATIO_ICO_BIG_DIGIT_WIDTH;
	
	drawDigit(10, true, base, RATIO_ICO_DISP_TIME_TOP, left);
	left += RATIO_ICO_COLON_WIDTH;
	
	drawDigit(minute / 10, true, base, RATIO_ICO_DISP_TIME_TOP, left);
	left += RATIO_ICO_BIG_DIGIT_WIDTH;
	drawDigit(minute % 10, true, base, RATIO_ICO_DISP_TIME_TOP, left);
	left += RATIO_ICO_BIG_DIGIT_WIDTH;

	drawDigit(10, true, base, RATIO_ICO_DISP_TIME_TOP, left);
	left += RATIO_ICO_COLON_WIDTH;

	drawDigit(second / 10, true, base, RATIO_ICO_DISP_TIME_TOP, left);
	left += RATIO_ICO_BIG_DIGIT_WIDTH;
	drawDigit(second % 10, true, base, RATIO_ICO_DISP_TIME_TOP, left);
}

static void drawCountDownSeconds(coords base, int timerId, float seconds) {
	coords tc, dc;

	tc.left = PIX_ICO_DN_LEFT / TEXTURE_WIDTH;
	tc.bottom = PIX_ICO_DN_TOP / TEXTURE_HEIGHT;
	tc.right = PIX_ICO_DN_RIGHT / TEXTURE_WIDTH;
	tc.top = PIX_ICO_DN_BOTTOM / TEXTURE_HEIGHT;

	float width = base.right - base.left + 1;
	float height = base.top - base.bottom + 1;

	dc.left = base.left + (RATIO_ICO_UP_LEFT2 + (float)timerId * RATIO_ICO_UP_SPACING)* width;
	dc.top = base.top - RATIO_ICO_UP_TOP * height;
	dc.right = base.left + (RATIO_ICO_UP_RIGHT2 + (float)timerId * RATIO_ICO_UP_SPACING) * width;
	dc.bottom = base.top - RATIO_ICO_UP_BOTTOM * height;

	drawTexture(tc, dc);

	int roundedSeconds = (int)ceilf(seconds);
	float left = RATIO_ICO_UP_LEFT2 + RATIO_ICO_SMALL_DIGIT_WIDTH + (float)timerId * RATIO_ICO_UP_SPACING;
	drawDigit(roundedSeconds/ 10, false, base, RATIO_ICO_SECONDS_TOP, left);
	left += RATIO_ICO_SMALL_DIGIT_WIDTH;
	drawDigit(roundedSeconds % 10, false, base, RATIO_ICO_SECONDS_TOP, left);
}

static void drawClock(float zuluTime, coords base, boolean isRoundUp) {
	int roundSeconds = isRoundUp ? (int)ceilf(zuluTime) : (int)zuluTime;
	int hr = roundSeconds / 3600;
	int min = (roundSeconds / 60) % 60;
	int sec = roundSeconds % 60;
	drawTime(hr, min, sec, base);
}

static void drawSet(coords base) {
	coords tc, dc;

	tc.left = PIX_ICO_SET_LEFT / TEXTURE_WIDTH;
	tc.bottom = PIX_ICO_SET_TOP / TEXTURE_HEIGHT;
	tc.right = PIX_ICO_SET_RIGHT / TEXTURE_WIDTH;
	tc.top = PIX_ICO_SET_BOTTOM / TEXTURE_HEIGHT;

	float width = base.right - base.left + 1;
	float height = base.top - base.bottom + 1;

	dc.left = base.left + RATIO_ICO_SET_LEFT * width;
	dc.top = base.top - RATIO_ICO_SET_TOP * height;
	dc.right = base.left + RATIO_ICO_SET_RIGHT * width;
	dc.bottom = base.top - RATIO_ICO_SET_BOTTOM * height;

	drawTexture(tc, dc);
}

static void drawUpArrow(coords base, int timerId) {
	coords tc, dc;

	tc.left = PIX_ICO_UP_LEFT / TEXTURE_WIDTH;
	tc.bottom = PIX_ICO_UP_TOP / TEXTURE_HEIGHT;
	tc.right = PIX_ICO_UP_RIGHT / TEXTURE_WIDTH;
	tc.top = PIX_ICO_UP_BOTTOM / TEXTURE_HEIGHT;

	float width = base.right - base.left + 1;
	float height = base.top - base.bottom + 1;

	dc.left = base.left + (RATIO_ICO_UP_LEFT  + (float)timerId * RATIO_ICO_UP_SPACING)* width;
	dc.top = base.top - RATIO_ICO_UP_TOP * height;
	dc.right = base.left + (RATIO_ICO_UP_RIGHT + (float)timerId * RATIO_ICO_UP_SPACING) * width;
	dc.bottom = base.top - RATIO_ICO_UP_BOTTOM * height;

	drawTexture(tc, dc);
}

static void drawDnArrow(coords base, int timerId) {
	coords tc, dc;

	tc.left = PIX_ICO_DN_LEFT / TEXTURE_WIDTH;
	tc.bottom = PIX_ICO_DN_TOP / TEXTURE_HEIGHT;
	tc.right = PIX_ICO_DN_RIGHT / TEXTURE_WIDTH;
	tc.top = PIX_ICO_DN_BOTTOM / TEXTURE_HEIGHT;

	float width = base.right - base.left + 1;
	float height = base.top - base.bottom + 1;

	dc.left = base.left + (RATIO_ICO_UP_LEFT + (float)timerId * RATIO_ICO_UP_SPACING)* width;
	dc.top = base.top - RATIO_ICO_UP_TOP * height;
	dc.right = base.left + (RATIO_ICO_UP_RIGHT + (float)timerId * RATIO_ICO_UP_SPACING) * width;
	dc.bottom = base.top - RATIO_ICO_UP_BOTTOM * height;

	drawTexture(tc, dc);
}

static void drawDnUpArrow(coords base, int timerId) {
	coords tc, dc;

	tc.left = PIX_ICO_DNUP_LEFT / TEXTURE_WIDTH;
	tc.bottom = PIX_ICO_DNUP_TOP / TEXTURE_HEIGHT;
	tc.right = PIX_ICO_DNUP_RIGHT / TEXTURE_WIDTH;
	tc.top = PIX_ICO_DNUP_BOTTOM / TEXTURE_HEIGHT;

	float width = base.right - base.left + 1;
	float height = base.top - base.bottom + 1;

	dc.left = base.left + (RATIO_ICO_DNUP_LEFT + (float)timerId * RATIO_ICO_UP_SPACING)* width;
	dc.top = base.top - RATIO_ICO_DNUP_TOP * height;
	dc.right = base.left + (RATIO_ICO_DNUP_RIGHT + (float)timerId * RATIO_ICO_UP_SPACING) * width;
	dc.bottom = base.top - RATIO_ICO_DNUP_BOTTOM * height;

	drawTexture(tc, dc);
}

static void drawClose(coords base) {
	coords tc, dc;

	tc.left = PIX_ICO_CLOSE_LEFT / TEXTURE_WIDTH;
	tc.bottom = PIX_ICO_CLOSE_TOP / TEXTURE_HEIGHT;
	tc.right = PIX_ICO_CLOSE_RIGHT / TEXTURE_WIDTH;
	tc.top = PIX_ICO_CLOSE_BOTTOM / TEXTURE_HEIGHT;

	float width = base.right - base.left + 1;
	float height = base.top - base.bottom + 1;

	dc.left = base.left + RATIO_ICO_CLOSE_LEFT * width;
	dc.top = base.top - RATIO_ICO_CLOSE_TOP * height;
	dc.right = base.left + RATIO_ICO_CLOSE_RIGHT * width;
	dc.bottom = base.top - RATIO_ICO_CLOSE_BOTTOM * height;

	drawTexture(tc, dc);
}

static void drawHand() {
	coords tc, dc;

	tc.left = PIX_ICO_HAND_LEFT / TEXTURE_WIDTH;
	tc.bottom = PIX_ICO_HAND_TOP / TEXTURE_HEIGHT;
	tc.right = PIX_ICO_HAND_RIGHT / TEXTURE_WIDTH;
	tc.top = PIX_ICO_HAND_BOTTOM / TEXTURE_HEIGHT;

	int x, y;
	XPLMGetMouseLocationGlobal(&x, &y);

	dc.left = x - 24.0f;
	dc.top = y + 24.0f;
	dc.right = dc.left + 48.0f;
	dc.bottom = dc.top - 48.0f;

	drawTexture(tc, dc);
}

static void drawHandDrag() {
	coords tc, dc;

	tc.left = PIX_ICO_HANDDRAG_LEFT / TEXTURE_WIDTH;
	tc.bottom = PIX_ICO_HANDDRAG_TOP / TEXTURE_HEIGHT;
	tc.right = PIX_ICO_HANDDRAG_RIGHT / TEXTURE_WIDTH;
	tc.top = PIX_ICO_HANDDRAG_BOTTOM / TEXTURE_HEIGHT;

	int x, y;
	XPLMGetMouseLocationGlobal(&x, &y);

	dc.left = x - 24.0f;
	dc.top = y + 24.0f;
	dc.right = dc.left + 48.0f;
	dc.bottom = dc.top - 48.0f;

	drawTexture(tc, dc);
}

static void drawHandResize() {
	coords tc, dc;

	tc.left = PIX_ICO_HANDRESIZE_LEFT / TEXTURE_WIDTH;
	tc.bottom = PIX_ICO_HANDRESIZE_TOP / TEXTURE_HEIGHT;
	tc.right = PIX_ICO_HANDRESIZE_RIGHT / TEXTURE_WIDTH;
	tc.top = PIX_ICO_HANDRESIZE_BOTTOM / TEXTURE_HEIGHT;

	int x, y;
	XPLMGetMouseLocationGlobal(&x, &y);

	dc.left = x - 25.0f;
	dc.top = y + 25.0f;
	dc.right = dc.left + 50.0f;
	dc.bottom = dc.top - 50.0f;

	drawTexture(tc, dc);
}

void draw(XPLMWindowID window_id, void * in_refcon)
{
	XPLMSetGraphicsState(0, 1, 0, 0, 1, 0, 0);
	XPLMBindTexture2d(textureId, 0);
	drawBase(baseDrawCoords);
	if (isPwrOn) {
		drawBattery(baseDrawCoords);
		drawSelected(baseDrawCoords, selected);
		drawVolume(baseDrawCoords, volume);
		if (selected == SEL_ID_CLOCK) {
			drawClock(clockTime, baseDrawCoords, false);
		} else {
			int i = selected - 1;
			drawClock(timers[i].time, baseDrawCoords, timers[i].mode == MODE_ID_DOWN);
		}
		for (int i = 0; i < 3; i++) {
			if (timers[i].mode == MODE_ID_UP && timers[i].time > 0.0f) {
				drawUpArrow(baseDrawCoords, i);
				continue;
			}
			if (timers[i].mode == MODE_ID_UP_AFTER_DOWN) {
				drawDnUpArrow(baseDrawCoords, i);
				continue;
			}
			if (timers[i].mode == MODE_ID_DOWN) {
				if (timers[i].time > 60.0) {
					drawDnArrow(baseDrawCoords, i);
				}
				else {
					drawCountDownSeconds(baseDrawCoords, i, timers[i].time);
				}
				continue;
			}
		}
		if (isSet) {
			drawSet(baseDrawCoords);
		}
	}
	if (isDrawClose) {
		drawClose(baseDrawCoords);
	}
	switch (cursorType)
	{
		case CURSOR_ID_HAND:
			drawHand();
			break;
		case CURSOR_ID_HANDDRAG:
			drawHandDrag();
			break;
		case CURSOR_ID_HANDRESIZE:
			drawHandResize();
			break;
		default:
			break;
	}
	//XPLMSetGraphicsState(0, 0, 0, 0, 1, 1, 0);
	//drawPressedButton(buttonPressed, baseDrawCoords);
}

int loadTexture(char *textureName)
{
	XPLMGenerateTextureNumbers(&textureId, 1);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	pngInfo info;
	return pngLoad(textureName, PNG_NOMIPMAP, PNG_ALPHA, &info);
}

