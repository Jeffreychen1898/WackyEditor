#include <stdio.h>
#include <stdlib.h>

#include <math.h>

#include "textbuffer.h"
#include "display.h"
#include "syntaxReader.h"
#include "dfa.h"
#include "charbuffer.h"

#define DEBUG_WIN_HEIGHT 10

/*
to do:

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
	dfa_init();
	disp_initialize();
	debug_init(5);
	syntax_initialize();

	struct TextBuffer* textbuf = textbuf_init(5, 5);

	CharBuffer* charbuf = charbuf_create();
	charbuf_setSpecialChars(charbuf, "/\"'*\n", 5);

	disp_update();

	struct ContentDisplay content_disp = {
		disp_width(),
		disp_height() - 3 - DEBUG_WIN_HEIGHT,
		0,
		0
	};

	/* load file begin, for stress testing, will handle files next time */
#if 1
	FILE* fp;
	char line[1024];

	fp = fopen("test2.test.c", "r");
	if(fp == NULL)
		return 2;
	while(fgets(line, sizeof(line), fp) != NULL)
	{
		int idx = 0;
		while(line[idx] != '\n' && line[idx] != '\0')
		{
			// load the char
			char mychar = line[idx];
			if(mychar == '\t')
			{
				mychar = ' ';
			}

			textbuf_addChar(textbuf, mychar);
			charbuf_insert(charbuf, mychar);
			++ idx;
		}
		// load the \n
		if(line[idx] == '\n')
		{
			textbuf_addChar(textbuf, '\n');
			charbuf_insert(charbuf, '\n');
		}
	}
	fclose(fp);
#endif
	/* load file end */

	disp_render(textbuf, &content_disp, charbuf);

	while(true)
	{
		chtype currchar = getch();
		
		debug_clear();

		if(currchar == KEY_LEFT)
		{
			textbuf_cursorLeft(textbuf);
			charbuf_left(charbuf);
		} else if(currchar == KEY_RIGHT)
		{
			textbuf_cursorRight(textbuf);
			charbuf_right(charbuf);
		} else if(currchar == KEY_UP)
		{
			textbuf_cursorUp(textbuf);
			charbuf_up(charbuf, textbuf_cursorPos(textbuf));
		} else if(currchar == KEY_DOWN)
		{
			textbuf_cursorDown(textbuf);
			charbuf_down(charbuf, textbuf_cursorPos(textbuf));
		}

		if((currchar >= ' ' && currchar <= '~') || currchar == '\n')
		{
			textbuf_addChar(textbuf, (char)currchar);
			charbuf_insert(charbuf, (char)currchar);
		} else if(currchar == 127)
		{
			charbuf_remove(charbuf, textbuf_charAt(textbuf, textbuf_cursorPos(textbuf)));
			textbuf_removeChar(textbuf);
		} else if(currchar == 9)
		{
			for(uint32_t i=0;i<4;++i)
			{
				textbuf_addChar(textbuf, ' ');
				charbuf_insert(charbuf, ' ');
			}
		}

		if(currchar == 27) break;

		disp_update();

		uint32_t curr_row = textbuf_row(textbuf);
		uint32_t curr_col = textbuf_col(textbuf);

		uint32_t linenum_offset = log10(textbuf_lineCount(textbuf)) + 2;
		content_disp.width = disp_width() - linenum_offset;
		content_disp.height = disp_height() - 3 - DEBUG_WIN_HEIGHT;

		if(curr_row >= content_disp.offsetY + content_disp.height)
			content_disp.offsetY = curr_row - content_disp.height + 1;
		if(curr_row < content_disp.offsetY)
			content_disp.offsetY = curr_row;

		if(curr_col > content_disp.offsetX + content_disp.width - 1)
			content_disp.offsetX = curr_col - content_disp.width + 1;
		if(curr_col < content_disp.offsetX)
			content_disp.offsetX = curr_col;

		debug_write("Debug Window :P ", 16);

		disp_render(textbuf, &content_disp, charbuf);

		move(mainwinInfo.height - 1, mainwinInfo.width - 10);
		addch(currchar);
	}

	textbuf_free(textbuf);
	charbuf_free(charbuf);

	disp_terminate();
	debug_terminate();
	syntax_terminate();
	dfa_terminate();
	return 0;
}
