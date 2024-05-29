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

	init_pair(2, COLOR_RED, COLOR_BLACK);
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

		uint32_t highlight_color = HIGHLIGHT_NONE;
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

			if(highlight_color == HIGHLIGHT_NONE)
				highlight_color = disp_getWordColor(_buf, i);
			if(disp_keycharHashValue(curr_char) == UINT32_MAX)
				highlight_color = HIGHLIGHT_NONE;

			if(highlight_color == HIGHLIGHT_KEYWORD)
				attron(COLOR_PAIR(2));
			addch(display_char);
			if(highlight_color == HIGHLIGHT_KEYWORD)
				attroff(COLOR_PAIR(2));
		}
		row_begin += textbuf_rowSize(_buf, r) + 1;

		if(r - _contentDisp->offsetY > _contentDisp->height)
			break;
	}

	if(textbuf_charAt(_buf, cursor_index) == '\n' || textbuf_charAt(_buf, cursor_index) == '\0')
	{
		move(textbuf_row(_buf) - _contentDisp->offsetY + 1, textbuf_col(_buf) - _contentDisp->offsetX + linenum_offset);
		addch(' ' | A_STANDOUT);
	}
}

void disp_renderSubWindow(struct ContentDisplay* _contentDisp)
{
	uint32_t row = 3 + _contentDisp->height;
	uint32_t col = 0;
	for(uint32_t i=0;i<debug_strlen();++i)
	{
		char get_char = debug_getChar(i);
		if(get_char == '\n')
		{
			++ row;
			col = 0;
			continue;
		}

		move(row, col);
		addch(get_char);
		++ col;
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
	printw("%d:%d", textbuf_row(_buf) + 1, textbuf_col(_buf) + 1);
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

uint32_t disp_getWordColor(struct TextBuffer* _buf, uint32_t _idx)
{
	uint32_t word_len = 0;
	uint64_t hash_value = 0;

	// march to the beginning
	uint32_t curr_char_hash = disp_keycharHashValue(textbuf_charAt(_buf, _idx));
	while(curr_char_hash != UINT32_MAX && word_len < 10)
	{
		if(_idx == 0) break;

		++ word_len;
		-- _idx;
		curr_char_hash = disp_keycharHashValue(textbuf_charAt(_buf, _idx));
	}

	// we traversed one too far back
	if(curr_char_hash == UINT32_MAX)
		curr_char_hash = disp_keycharHashValue(textbuf_charAt(_buf, ++ _idx));

	// hash each char
	word_len = 0;
	while(curr_char_hash != UINT32_MAX)
	{
		if(word_len >= 10) return HIGHLIGHT_NONE;

		hash_value = (hash_value << 6) | curr_char_hash;

		++ word_len;
		++ _idx;
		curr_char_hash = disp_keycharHashValue(textbuf_charAt(_buf, _idx));
	}

	// perform binary search
	if(hash_value == 63003) return HIGHLIGHT_KEYWORD;
	
	return HIGHLIGHT_NONE;
}

uint32_t disp_keycharHashValue(char _c)
{
	if(_c >= '0' && _c <= '9')
		return (uint32_t)(_c - '0'); // 0-9 maps to 0-9
	if(_c >= 'a' && _c <= 'z')
		return (uint32_t)(_c - 'a') + 10; // a-z maps to 10-35
	if(_c >= 'A' && _c <= 'Z')
		return (uint32_t)(_c - 'A') + 36; // A-Z maps to 36-61
	
	if(_c == '_') return 62;
	if(_c == '$') return 63;

	return UINT32_MAX;
}

void disp_terminate()
{
	endwin();
}
