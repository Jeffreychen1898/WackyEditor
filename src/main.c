#include <stdio.h>

#include <math.h>

#include "textbuffer.h"
#include "display.h"

#define DEBUG_WIN_HEIGHT 10

/*
to do:

debug buffer
polishing
syntax highlighting
	keywords:
		strings
		data types
		numbers
collapsable debug buffer
command mode?
file loading
*/

extern struct WindowInfo mainwinInfo;

int main()
{
	disp_initialize();

	struct TextBuffer* textbuf = textbuf_init(5, 5);

	disp_update();

	struct ContentDisplay content_disp = {
		disp_width(),
		disp_height() - 3 - DEBUG_WIN_HEIGHT,
		0,
		0
	};

	disp_render(textbuf, &content_disp);

	while(true)
	{
		chtype currchar = getch();

		if(currchar == KEY_LEFT)
			textbuf_cursorLeft(textbuf);
		else if(currchar == KEY_RIGHT)
			textbuf_cursorRight(textbuf);
		else if(currchar == KEY_UP)
			textbuf_cursorUp(textbuf);
		else if(currchar == KEY_DOWN)
			textbuf_cursorDown(textbuf);

		if((currchar >= ' ' && currchar <= '~') || currchar == '\n')
			textbuf_addChar(textbuf, (char)currchar);
		else if(currchar == 127)
			textbuf_removeChar(textbuf);
		else if(currchar == 9)
		{
			for(uint32_t i=0;i<4;++i)
				textbuf_addChar(textbuf, ' ');
		}

		if(currchar == 27) break;

		disp_update();

		uint32_t curr_row = textbuf_row(textbuf);
		uint32_t curr_col = textbuf_col(textbuf);

		uint32_t linenum_offset = log10(textbuf_lineCount(textbuf)) + 2;
		content_disp.width = disp_width() - linenum_offset;
		content_disp.height = disp_height() - 3 - DEBUG_WIN_HEIGHT;

		if(curr_row > content_disp.offsetY + content_disp.height + 1)
			content_disp.offsetY = curr_row - content_disp.height - 1;
		if(curr_row < content_disp.offsetY)
			content_disp.offsetY = curr_row;

		if(curr_col > content_disp.offsetX + content_disp.width - 1)
			content_disp.offsetX = curr_col - content_disp.width + 1;
		if(curr_col < content_disp.offsetX)
			content_disp.offsetX = curr_col;

		disp_render(textbuf, &content_disp);

		move(mainwinInfo.height - 1, mainwinInfo.width - 10);
		addch(currchar);
	}

	textbuf_free(textbuf);

	disp_terminate();
	return 0;
}
