#include "display.h"

WINDOW* mainwin = NULL;
struct WindowInfo mainwinInfo = { 0, 0 };

void disp_initialize()
{
	mainwin = initscr();

	cbreak();
	noecho();
	curs_set(0);
	keypad(mainwin, TRUE);
	set_escdelay(0);

	start_color();
	//if(has_color() == FALSE) printf("pathetic");
	
	init_pair(1, COLOR_BLACK, COLOR_CYAN);
}

void disp_renderTopbar()
{
	// render top bar
	move(0, 1);
	printw("NO NAME");
}

void disp_renderContent(struct TextBuffer* _buf, struct ContentDisplay* _contentDisp)
{
	// apply some offset to the x for the line numbers
	int64_t linenum_offset = log10(textbuf_lineCount(_buf)) + 2;

	// render the content
	int64_t cursor_index = textbuf_cursorPos(_buf);
	int64_t row_begin = cursor_index - textbuf_col(_buf);

	// find the first row to render
	int64_t start_row = textbuf_row(_buf);
	while(start_row > _contentDisp->offsetY && start_row > 0)
	{
		-- start_row;
		row_begin -= textbuf_rowSize(_buf, start_row) + 1;
	}

	// render each row
	for(int64_t r=start_row;r<textbuf_lineCount(_buf);++r)
	{
		// render the line number
		move(r - _contentDisp->offsetY + 1, 0);
		printw("%d", r + 1);
		// render the content
		for(int64_t i=row_begin + _contentDisp->offsetX;i<textbuf_size(_buf);++i)
		{
			// out of bounds
			if(i - _contentDisp->offsetX - row_begin >= _contentDisp->width)
				break;
			if(i - row_begin >= textbuf_rowSize(_buf, r))
				break;
			
			// get the char
			char curr_char = textbuf_charAt(_buf, i);
			chtype display_char = (chtype)curr_char;

			// rendering the char
			move(r - _contentDisp->offsetY + 1, i - _contentDisp->offsetX - row_begin + linenum_offset);

			if(i == cursor_index)
				display_char |= A_STANDOUT;

			addch(display_char);
		}
		row_begin += textbuf_rowSize(_buf, r) + 1;

		if(r - _contentDisp->offsetY > _contentDisp->height)
			break;
	}

	if(textbuf_charAt(_buf, cursor_index) == '\n' || textbuf_charAt(_buf, cursor_index) == '\0')
	{
		move(_buf->currRow - _contentDisp->offsetY + 1, _buf->currCol - _contentDisp->offsetX + linenum_offset);
		addch(' ' | A_STANDOUT);
	}
}

void disp_renderSubWindow(struct ContentDisplay* _contentDisp)
{
	char debug_stuff[] = "dlkfjasldf\nasdfa\nasd\nfasd\nfasdf\nasdfasdf\nasdfasdf\nasdfasdfasdf\nasdfasdf\n";
	uint32_t r = 3 + _contentDisp->height;
	uint32_t c = 0;
	for(int32_t i=0;debug_stuff[i] != '\0' && r < disp_height() - 2;++i)
	{
		move(r, c);
		if(debug_stuff[i] == '\n')
		{
			++ r;
			c = 0;
			continue;
		} else
			addch(debug_stuff[i]);

		++ c;
	}
}

void disp_renderBottomBar(struct TextBuffer* _buf)
{
	// render the bottom info bar
	attron(COLOR_PAIR(1));
	for(uint32_t i=0;i<mainwinInfo.width;++i)
	{
		move(mainwinInfo.height - 2, i);
		addch(' ');
	}
	move(mainwinInfo.height - 2, mainwinInfo.width - 15);
	printw("%d:%d", _buf->currRow + 1, _buf->currCol + 1);
	attroff(COLOR_PAIR(1));
}

void disp_render(struct TextBuffer* _buf, struct ContentDisplay* _contentDisp)
{
	disp_clear();

	disp_renderTopbar();
	disp_renderContent(_buf, _contentDisp);
	disp_renderSubWindow(_contentDisp);
	disp_renderBottomBar(_buf);

	disp_refresh();
}

void disp_update()
{
	getmaxyx(mainwin, mainwinInfo.height, mainwinInfo.width);
}

void disp_terminate()
{
	endwin();
}
