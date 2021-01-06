#pragma once

#if LIN | APL
	#include <stdbool.h>
	typedef bool boolean;
	typedef unsigned char byte;
#endif

typedef struct {
	boolean isRunning;
	byte mode;
	float time;
	float recall;
} timer;


typedef struct {
	float left;
	float right;
	float top;
	float bottom;
} coords;
