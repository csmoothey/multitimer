#if IBM
	#include <windows.h>
#endif
#if APL
   #include <OpenGl/gl.h>
#else
   #include <GL/gl.h>
#endif
#include "metrics.h"
#include "XPLMGraphics.h"
#include "types.h"
#include "buttonid.h"

static float red[] = { 1.0, 0.0, 0.0, 1.0 };

static void drawBtnClockBoundary(float l, float t, float width, float height) {
	glColor4fv(red);
	glBegin(GL_LINE_LOOP);
	glVertex2f(l + RATIO_BTN_CLOCK_LEFT * width, t - RATIO_BTN_CLOCK_TOP * height);
	glVertex2f(l + RATIO_BTN_CLOCK_RIGHT * width, t - RATIO_BTN_CLOCK_TOP * height);
	glVertex2f(l + RATIO_BTN_CLOCK_RIGHT * width, t - RATIO_BTN_CLOCK_BOTTOM * height);
	glVertex2f(l + RATIO_BTN_CLOCK_LEFT * width, t - RATIO_BTN_CLOCK_BOTTOM * height);
	glEnd();

}

static void drawBtnTimer1Boundary(float l, float t, float width, float height) {
	glColor4fv(red);
	glBegin(GL_LINE_LOOP);
	glVertex2f(l + RATIO_BTN_TIMER1_LEFT * width, t - RATIO_BTN_TIMER1_TOP * height);
	glVertex2f(l + RATIO_BTN_TIMER1_RIGHT * width, t - RATIO_BTN_TIMER1_TOP * height);
	glVertex2f(l + RATIO_BTN_TIMER1_RIGHT * width, t - RATIO_BTN_TIMER1_BOTTOM * height);
	glVertex2f(l + RATIO_BTN_TIMER1_LEFT * width, t - RATIO_BTN_TIMER1_BOTTOM * height);
	glEnd();

}

static void drawBtnTimer2Boundary(float l, float t, float width, float height) {
	glColor4fv(red);
	glBegin(GL_LINE_LOOP);
	glVertex2f(l + RATIO_BTN_TIMER2_LEFT * width, t - RATIO_BTN_TIMER2_TOP * height);
	glVertex2f(l + RATIO_BTN_TIMER2_RIGHT * width, t - RATIO_BTN_TIMER2_TOP * height);
	glVertex2f(l + RATIO_BTN_TIMER2_RIGHT * width, t - RATIO_BTN_TIMER2_BOTTOM * height);
	glVertex2f(l + RATIO_BTN_TIMER2_LEFT * width, t - RATIO_BTN_TIMER2_BOTTOM * height);
	glEnd();

}

static void drawBtnTimer3Boundary(float l, float t, float width, float height) {
	glColor4fv(red);
	glBegin(GL_LINE_LOOP);
	glVertex2f(l + RATIO_BTN_TIMER3_LEFT * width, t - RATIO_BTN_TIMER3_TOP * height);
	glVertex2f(l + RATIO_BTN_TIMER3_RIGHT * width, t - RATIO_BTN_TIMER3_TOP * height);
	glVertex2f(l + RATIO_BTN_TIMER3_RIGHT * width, t - RATIO_BTN_TIMER3_BOTTOM * height);
	glVertex2f(l + RATIO_BTN_TIMER3_LEFT * width, t - RATIO_BTN_TIMER3_BOTTOM * height);
	glEnd();

}

static void drawBtnPwrBoundary(float l, float t, float width, float height) {
	glColor4fv(red);
	glBegin(GL_LINE_LOOP);
	glVertex2f(l + RATIO_BTN_PWR_LEFT * width, t - RATIO_BTN_PWR_TOP * height);
	glVertex2f(l + RATIO_BTN_PWR_RIGHT * width, t - RATIO_BTN_PWR_TOP * height);
	glVertex2f(l + RATIO_BTN_PWR_RIGHT * width, t - RATIO_BTN_PWR_BOTTOM * height);
	glVertex2f(l + RATIO_BTN_PWR_LEFT * width, t - RATIO_BTN_PWR_BOTTOM * height);
	glEnd();

}

static void drawBtnVolBoundary(float l, float t, float width, float height) {
	glColor4fv(red);
	glBegin(GL_LINE_LOOP);
	glVertex2f(l + RATIO_BTN_VOL_LEFT * width, t - RATIO_BTN_VOL_TOP * height);
	glVertex2f(l + RATIO_BTN_VOL_RIGHT * width, t - RATIO_BTN_VOL_TOP * height);
	glVertex2f(l + RATIO_BTN_VOL_RIGHT * width, t - RATIO_BTN_VOL_BOTTOM * height);
	glVertex2f(l + RATIO_BTN_VOL_LEFT * width, t - RATIO_BTN_VOL_BOTTOM * height);
	glEnd();

}

static void drawBtnStsBoundary(float l, float t, float width, float height) {
	glColor4fv(red);
	glBegin(GL_LINE_LOOP);
	glVertex2f(l + RATIO_BTN_STS_LEFT * width, t - RATIO_BTN_STS_TOP * height);
	glVertex2f(l + RATIO_BTN_STS_RIGHT * width, t - RATIO_BTN_STS_TOP * height);
	glVertex2f(l + RATIO_BTN_STS_RIGHT * width, t - RATIO_BTN_STS_BOTTOM * height);
	glVertex2f(l + RATIO_BTN_STS_LEFT * width, t - RATIO_BTN_STS_BOTTOM * height);
	glEnd();

}

static void drawBtnHrBoundary(float l, float t, float width, float height) {
	glColor4fv(red);
	glBegin(GL_LINE_LOOP);
	glVertex2f(l + RATIO_BTN_HR_LEFT * width, t - RATIO_BTN_HR_TOP * height);
	glVertex2f(l + RATIO_BTN_HR_RIGHT * width, t - RATIO_BTN_HR_TOP * height);
	glVertex2f(l + RATIO_BTN_HR_RIGHT * width, t - RATIO_BTN_HR_BOTTOM * height);
	glVertex2f(l + RATIO_BTN_HR_LEFT * width, t - RATIO_BTN_HR_BOTTOM * height);
	glEnd();

}

static void drawBtnMinBoundary(float l, float t, float width, float height) {
	glColor4fv(red);
	glBegin(GL_LINE_LOOP);
	glVertex2f(l + RATIO_BTN_MIN_LEFT * width, t - RATIO_BTN_MIN_TOP * height);
	glVertex2f(l + RATIO_BTN_MIN_RIGHT * width, t - RATIO_BTN_MIN_TOP * height);
	glVertex2f(l + RATIO_BTN_MIN_RIGHT * width, t - RATIO_BTN_MIN_BOTTOM * height);
	glVertex2f(l + RATIO_BTN_MIN_LEFT * width, t - RATIO_BTN_MIN_BOTTOM * height);
	glEnd();

}

static void drawBtnSecBoundary(float l, float t, float width, float height) {
	glColor4fv(red);
	glBegin(GL_LINE_LOOP);
	glVertex2f(l + RATIO_BTN_SEC_LEFT * width, t - RATIO_BTN_SEC_TOP * height);
	glVertex2f(l + RATIO_BTN_SEC_RIGHT * width, t - RATIO_BTN_SEC_TOP * height);
	glVertex2f(l + RATIO_BTN_SEC_RIGHT * width, t - RATIO_BTN_SEC_BOTTOM * height);
	glVertex2f(l + RATIO_BTN_SEC_LEFT * width, t - RATIO_BTN_SEC_BOTTOM * height);
	glEnd();

}

static void drawBtnClrBoundary(float l, float t, float width, float height) {
	glColor4fv(red);
	glBegin(GL_LINE_LOOP);
	glVertex2f(l + RATIO_BTN_CLR_LEFT * width, t - RATIO_BTN_CLR_TOP * height);
	glVertex2f(l + RATIO_BTN_CLR_RIGHT * width, t - RATIO_BTN_CLR_TOP * height);
	glVertex2f(l + RATIO_BTN_CLR_RIGHT * width, t - RATIO_BTN_CLR_BOTTOM * height);
	glVertex2f(l + RATIO_BTN_CLR_LEFT * width, t - RATIO_BTN_CLR_BOTTOM * height);
	glEnd();

}

static void drawBtnRclBoundary(float l, float t, float width, float height) {
	glColor4fv(red);
	glBegin(GL_LINE_LOOP);
	glVertex2f(l + RATIO_BTN_RCL_LEFT * width, t - RATIO_BTN_RCL_TOP * height);
	glVertex2f(l + RATIO_BTN_RCL_RIGHT * width, t - RATIO_BTN_RCL_TOP * height);
	glVertex2f(l + RATIO_BTN_RCL_RIGHT * width, t - RATIO_BTN_RCL_BOTTOM * height);
	glVertex2f(l + RATIO_BTN_RCL_LEFT * width, t - RATIO_BTN_RCL_BOTTOM * height);
	glEnd();

}

static void drawBtnCloseBoundary(float l, float t, float width, float height) {
	glColor4fv(red);
	glBegin(GL_LINE_LOOP);
	glVertex2f(l + RATIO_ICO_CLOSE_LEFT * width, t - RATIO_ICO_CLOSE_TOP * height);
	glVertex2f(l + RATIO_ICO_CLOSE_RIGHT * width, t - RATIO_ICO_CLOSE_TOP * height);
	glVertex2f(l + RATIO_ICO_CLOSE_RIGHT * width, t - RATIO_ICO_CLOSE_BOTTOM * height);
	glVertex2f(l + RATIO_ICO_CLOSE_LEFT * width, t - RATIO_ICO_CLOSE_BOTTOM * height);
	glEnd();

}

void drawPressedButton(byte buttonPressed, coords baseDrawCoords) {
	float width = baseDrawCoords.right - baseDrawCoords.left + 1;
	float height = baseDrawCoords.top - baseDrawCoords.bottom + 1;

	switch (buttonPressed) {
	case BTN_ID_NONE:
		break;
	case BTN_ID_CLOCK:
		drawBtnClockBoundary(baseDrawCoords.left, baseDrawCoords.top, width, height);
		break;
	case BTN_ID_TIMER1:
		drawBtnTimer1Boundary(baseDrawCoords.left, baseDrawCoords.top, width, height);
		break;
	case BTN_ID_TIMER2:
		drawBtnTimer2Boundary(baseDrawCoords.left, baseDrawCoords.top, width, height);
		break;
	case BTN_ID_TIMER3:
		drawBtnTimer3Boundary(baseDrawCoords.left, baseDrawCoords.top, width, height);
		break;
	case BTN_ID_PWR:
		drawBtnPwrBoundary(baseDrawCoords.left, baseDrawCoords.top, width, height);
		break;
	case BTN_ID_VOL:
		drawBtnVolBoundary(baseDrawCoords.left, baseDrawCoords.top, width, height);
		break;
	case BTN_ID_STS:
		drawBtnStsBoundary(baseDrawCoords.left, baseDrawCoords.top, width, height);
		break;
	case BTN_ID_HR:
		drawBtnHrBoundary(baseDrawCoords.left, baseDrawCoords.top, width, height);
		break;
	case BTN_ID_MIN:
		drawBtnMinBoundary(baseDrawCoords.left, baseDrawCoords.top, width, height);
		break;
	case BTN_ID_SEC:
		drawBtnSecBoundary(baseDrawCoords.left, baseDrawCoords.top, width, height);
		break;
	case BTN_ID_CLR:
		drawBtnClrBoundary(baseDrawCoords.left, baseDrawCoords.top, width, height);
		break;
	case BTN_ID_RCL:
		drawBtnRclBoundary(baseDrawCoords.left, baseDrawCoords.top, width, height);
		break;
	case BTN_ID_CLOSE:
		drawBtnCloseBoundary(baseDrawCoords.left, baseDrawCoords.top, width, height);
		break;
	default:
		break;
	}
}
