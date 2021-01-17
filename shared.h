#pragma once
#if APL
    #include <Carbon/Carbon.h>
#endif

void changeClockTime(float delta);
void initDevice();
void destroyWindow();
void getPluginDir(char * buf);
#if APL
    int ConvertPath(const char * inPath, char * outPath, int outPathMaxLen);
#endif
