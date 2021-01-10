#if LIN | APL
	#include <stddef.h>
	#include <stdlib.h>
	#include <string.h>
#endif
#include "drawcore.h"
#include "state.h"
#include "mouse.h"
#include "flightloop.h"
#include "buttonid.h"
#include "cursorid.h"
#include "soundcore.h"
#include "shared.h"

static int				dummy_mouse_handler(XPLMWindowID window_id, int x, int y, int is_down, void * in_refcon) { return 0; }
static int				dummy_wheel_handler(XPLMWindowID window_id, int x, int y, int wheel, int clicks, void * in_refcon) { return 0; }
static void				dummy_key_handler(XPLMWindowID window_id, char key, XPLMKeyFlags flags, char virtual_key, void * in_refcon, int losing_focus) { }
static void				dummy_menu_handler(void * in_menu_ref, void * in_item_ref) {}

XPLMWindowID windowId;
XPLMDataRef drZuluTime;
XPLMDataRef drLocalTime;
XPLMDataRef drIsInReplay;
coords baseDrawCoords;
byte buttonPressed = 0;
boolean isPwrOn = false;
byte selected = 0;
byte volume = 3;
byte brightness = 0;
float clockTime = 0.0;
float zuluTime = 0.0;
boolean isSet = false;
struct timeb clockPressedAt;
timer timers[3];
boolean isWindowVisible = false;
boolean isDrag = false;
int mouseX, mouseY;
boolean isDrawClose = false;
byte cursorType = 0;
boolean isResize = false;

XPLMFlightLoopID timerFloopId;
XPLMFlightLoopID mouseFloopId;
XPLMCommandRef commandId;
XPLMCommandRef commandId2;
XPLMMenuID g_menu_id;
XPLMMenuID g_menu_id2;

static void createWindow() {
	int global_desktop_bounds[4]; // left, bottom, right, top
	XPLMGetScreenBoundsGlobal(&global_desktop_bounds[0], &global_desktop_bounds[3], &global_desktop_bounds[2], &global_desktop_bounds[1]);

	XPLMCreateWindow_t params;
	params.structSize = sizeof(params);
	params.left = (int)baseDrawCoords.left;
	params.bottom = (int)baseDrawCoords.bottom;
	params.right = (int)baseDrawCoords.right;
	params.top = (int)baseDrawCoords.top;
	params.visible = 1;
	params.drawWindowFunc = draw;
	params.handleMouseClickFunc = handleMouseClick;
	params.handleRightClickFunc = dummy_mouse_handler;
	params.handleMouseWheelFunc = dummy_wheel_handler;
	params.handleKeyFunc = dummy_key_handler;
	params.handleCursorFunc = handleCursor;
	params.refcon = NULL;
	params.layer = xplm_WindowLayerFloatingWindows;
	params.decorateAsFloatingWindow = xplm_WindowDecorationSelfDecorated;

	windowId = XPLMCreateWindowEx(&params);
	XPLMSetWindowPositioningMode(windowId, xplm_WindowPositionFree, -1);

	buttonPressed = BTN_ID_NONE;
	cursorType = CURSOR_ID_DEFAULT;
	isDrawClose = false;
}

static int commandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void * inRefcon) {
	if (inPhase == 0)
	{
		isWindowVisible = !isWindowVisible;
		if (isWindowVisible) {
			createWindow();
		} else {
			destroyWindow();
		}
	}

	return 0;
}

static int commandHandler2(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void * inRefcon) {
	if (inPhase == 0)
	{
		brightness = (brightness + 1) % 7;
	}

	return 0;
}

static void	brightness_menu_handler(void * in_menu_ref, void * in_item_ref) {
	brightness = -atoi((const char *)in_item_ref);
}


PLUGIN_API int XPluginStart(char * outName, char * outSig, char * outDesc) {
	strcpy(outName, "MultiTimer");
	strcpy(outSig, "craigsmoothey.multitimer");
	strcpy(outDesc, "A plugin providing a multi-timer");

	char pluginDir[2048];
	getPluginDir(pluginDir);

	char overlayFile[2048];
	strcpy(overlayFile, pluginDir);
	strcat(overlayFile, "timeroverlay.png");

	if (!loadTexture(overlayFile)) {
		XPLMDebugString("MultiTimer: Error loading the overlay file");
		XPLMDebugString(overlayFile);
		return 0;
	}

	drZuluTime = XPLMFindDataRef("sim/time/zulu_time_sec");
	drLocalTime = XPLMFindDataRef("sim/time/local_time_sec");
	drIsInReplay = XPLMFindDataRef("sim/time/is_in_replay");

	commandId = XPLMCreateCommand("MultiTimer/ToggleDisplay", "Toggle display of the MultiTimer window");
	XPLMRegisterCommandHandler(commandId, commandHandler, 1, (void *)0);

	commandId2 = XPLMCreateCommand("MultiTimer/Brightness", "Change brightness of the MultiTimer window");
	XPLMRegisterCommandHandler(commandId2, commandHandler2, 1, (void *)0);

	int g_menu_container_idx = XPLMAppendMenuItem(XPLMFindPluginsMenu(), "MultiTimer", 0, 0);
	g_menu_id = XPLMCreateMenu("MultiTimer", XPLMFindPluginsMenu(), g_menu_container_idx, dummy_menu_handler, NULL);
	XPLMAppendMenuItemWithCommand(g_menu_id, "Toggle display", XPLMFindCommand("MultiTimer/ToggleDisplay"));
	int g_menu_container_idx2 = XPLMAppendMenuItem(g_menu_id, "Window brightness", 0, 0);
	g_menu_id2 = XPLMCreateMenu("Window brightness", g_menu_id, g_menu_container_idx2, brightness_menu_handler, NULL);
	XPLMAppendMenuItem(g_menu_id2, " 0", (void *)" 0", 1);
	XPLMAppendMenuItem(g_menu_id2, "-1", (void *)"-1", 1);
	XPLMAppendMenuItem(g_menu_id2, "-2", (void *)"-2", 1);
	XPLMAppendMenuItem(g_menu_id2, "-3", (void *)"-3", 1);
	XPLMAppendMenuItem(g_menu_id2, "-4", (void *)"-4", 1);
	XPLMAppendMenuItem(g_menu_id2, "-5", (void *)"-5", 1);
	XPLMAppendMenuItem(g_menu_id2, "-6", (void *)"-6", 1);

	XPLMRegisterFlightLoopCallback(openSound, -1.0, NULL);
	return 1;
}

PLUGIN_API void	XPluginStop(void) {
	XPLMUnregisterCommandHandler(commandId, commandHandler, 0, 0);
	XPLMUnregisterCommandHandler(commandId2, commandHandler2, 0, 0);
	XPLMDestroyMenu(g_menu_id2);
	XPLMDestroyMenu(g_menu_id);
	closeSound();
}

PLUGIN_API int XPluginEnable(void) {
	initDevice();
	XPLMCreateFlightLoop_t flightLoop_t = { sizeof(XPLMCreateFlightLoop_t), xplm_FlightLoop_Phase_AfterFlightModel, timerFloopCb, NULL };
	timerFloopId = XPLMCreateFlightLoop(&flightLoop_t);
	XPLMScheduleFlightLoop(timerFloopId, 0.1f, false);
	flightLoop_t.callbackFunc = mouseFloopCb;
	mouseFloopId = XPLMCreateFlightLoop(&flightLoop_t);
	XPLMScheduleFlightLoop(mouseFloopId, 0.25f, false);

	int global_desktop_bounds[4]; // left, bottom, right, top
	XPLMGetScreenBoundsGlobal(&global_desktop_bounds[0], &global_desktop_bounds[3], &global_desktop_bounds[2], &global_desktop_bounds[1]);
	baseDrawCoords.left = global_desktop_bounds[0] + 50.0f;
	baseDrawCoords.bottom = global_desktop_bounds[1] + 150.0f;
	baseDrawCoords.right = global_desktop_bounds[0] + 568.0f;
	baseDrawCoords.top = global_desktop_bounds[1] + 384.0f;

	return 1;
}

PLUGIN_API void XPluginDisable(void) {
	XPLMDestroyFlightLoop(timerFloopId);
	timerFloopId = NULL;
	XPLMDestroyFlightLoop(mouseFloopId);
	mouseFloopId = NULL;
	if (isWindowVisible) {
		destroyWindow();
	}
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFrom, int inMsg, void * inParam) { }
