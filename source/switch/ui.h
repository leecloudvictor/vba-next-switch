#ifndef __UI_H__
#define __UI_H__

typedef enum {
	resultSelectedFile,
	resultClose,
	resultUnpause,
	resultExit,
	resultNone,
	resultLoadState,
	resultSaveState,
	resultIncFrameskip
} UIResult;

typedef enum { stateRunning, stateFileselect, statePaused } UIState;

#define PATH_LENGTH 512

void uiInit();
void uiDeinit();

void uiGetSelectedFile(char* out);

UIResult uiLoop(u8* fb, u32 fbWidth, u32 fbHeight, u32 keysDown);
void uiSetState(UIState state);

void uiStatusMsg(const char* fmt, ...);

void uiFill(u8* fb, u16 fbWidth, u16 fbHeight, int x, int y, int width, int height, u8 r, u8 g, u8 b);
void uiDrawString(u8* fb, u16 fbWidth, u16 fbHeight, const char* string, int x, int y, u8 r, u8 g, u8 b);

#endif