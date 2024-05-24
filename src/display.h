#ifndef WACK_DISPLAY_H
#define WACK_DISPLAY_H

#include <ncurses.h>
#include <stdint.h>
#include <math.h>

#include "textbuffer.h"

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

void disp_initialize();
void disp_terminate();

inline void disp_clear() { clear(); };
inline void disp_refresh() { refresh(); };
void disp_renderTopbar();
void disp_renderContent(struct TextBuffer* _buf, struct ContentDisplay* _contentDisp);
void disp_renderSubWindow();
void disp_renderBottomBar(struct TextBuffer* _buf);

void disp_render(struct TextBuffer* _buf, struct ContentDisplay* _contentDisp);

void disp_update();

inline uint32_t disp_width() { return mainwinInfo.width; };
inline uint32_t disp_height() { return mainwinInfo.height; };

#endif
