#ifndef WACK_DISPLAY_H
#define WACK_DISPLAY_H

#include <ncurses.h>
#include <stdint.h>
#include <math.h>

#include "textbuffer.h"
#include "debugBuffer.h"
#include "syntaxReader.h"
#include "charbuffer.h"

#include "dfa.h"

#define HIGHLIGHT_NONE 0
#define HIGHLIGHT_KEYWORD 1

/*
 * create a gap buffer of state changes
 *
 * add char -> changes state? add state at cursor, set terminal
 * delete char -> if equal to state change, remove state change, set terminal
 *
 * for each line
 *		if(first char index < terminate)
 *			bst to get to first index
 *		else
 *			use this index: updateToHere(idx)
 *
 *		for each char in line
 *			if char index < terminal:
 *				handle as normally
 *
 *		get the start first state
 *		traverse through essential chars starting there, get to first char
 *		traverse until end of line
 *		rince and repeat
*/

struct WindowInfo
{
	uint32_t width;
	uint32_t height;
};

struct ContentDisplay
{
	uint32_t width;
	uint32_t height;
	int32_t offsetX;
	int32_t offsetY;
};

extern WINDOW* mainwin;
extern struct WindowInfo mainwinInfo;

extern DFADecider* decider;

void disp_initialize();
void disp_terminate();

inline void disp_clear() { clear(); };
inline void disp_refresh() { refresh(); };
void disp_renderTopbar();
void disp_renderContent(struct TextBuffer* _buf, struct ContentDisplay* _contentDisp, CharBuffer* _charbuf);
void disp_renderSubWindow();
void disp_renderBottomBar(struct TextBuffer* _buf);

void disp_render(struct TextBuffer* _buf, struct ContentDisplay* _contentDisp, CharBuffer* _charbuf);

void disp_update();

uint32_t disp_getWordColor(struct TextBuffer* _buf, uint32_t _idx);
uint32_t disp_keycharHashValue(char _c);

inline uint32_t disp_width() { return mainwinInfo.width; };
inline uint32_t disp_height() { return mainwinInfo.height; };

#endif
