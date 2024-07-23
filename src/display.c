#include "display.h"

WINDOW* mainwin = NULL;
struct WindowInfo mainwinInfo = { 0, 0 };

DFADecider* decider;

void disp_initialize()
{
	decider = dfa_compile("N[N:,C:/,D:\",Q:']C[N:,S:/,M:*]M[M:,E:*]E[M:,N:/]S[S:,N:\n]D[D:,N:\"]Q[Q:,N:']");

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

	init_pair(3, COLOR_GREEN, COLOR_BLACK);
	init_pair(4, COLOR_YELLOW, COLOR_BLACK);
}

void disp_renderTopbar()
{
	// render top bar
	move(0, 1);
	printw("NO NAME");
}

void disp_renderContent(struct TextBuffer* _buf, struct ContentDisplay* _contentDisp, CharBuffer* _charbuf)
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

	uint32_t dfa_idx = 0;
	DFAState* dfa_currstate = dfa_getState(decider, 'N');
	uint32_t charbuf_idx = 0;

	{
		//LineState get_line_state = textbuf_requestState(_buf, r);
		//dfa_idx = get_line_state.index;
		//dfa_currstate = dfa_getState(decider, get_line_state.state);
		//charbuf_idx = get_line_state.charbufidx;

		// if char == \0

		// insert base state
		textbuf_removeState(_buf, start_row);
		LineState prev_state = textbuf_requestState(_buf, start_row);

		// for debugging
		//textbuf_removeState(_buf, 0);
		//prev_state = textbuf_requestState(_buf, start_row);

		if(prev_state.state == '\0')
			textbuf_insertState(_buf, 'N', 0, 0, 0);
		else
		{
			dfa_idx = prev_state.index;
			dfa_currstate = dfa_getState(decider, prev_state.state);
			charbuf_idx = prev_state.charbufidx;
		}

		//move(30, 30);
		//printw("%d", prev_state.line + 1);
		
		uint32_t line_idx = dfa_idx + 1;

		for(uint32_t i=prev_state.line;i<start_row;++i)
		{

			uint32_t col_count = textbuf_rowSize(_buf, i);
			uint32_t next_col_idx = line_idx + col_count + 1;
			line_idx += col_count + 1;

			uint32_t next_pos = gapBuf_uintAt(_charbuf->charIndex, charbuf_idx);
			if(charbuf_idx > 0) next_pos ++;

			while(dfa_idx + next_pos < next_col_idx)
			{
				dfa_idx += next_pos;

				char next_char = textbuf_charAt(_buf, dfa_idx);
				char state_label = dfa_currstate->label;
				if(next_pos > 1 && (state_label == 'C' || state_label == 'E'))
					dfa_currstate = dfa_nextState(decider, dfa_currstate, '\0');
				dfa_currstate = dfa_nextState(decider, dfa_currstate, next_char);
				++ charbuf_idx;

				next_pos = gapBuf_uintAt(_charbuf->charIndex, charbuf_idx) + 1;
			}

			// cache line i + 1
			textbuf_insertState(_buf, dfa_currstate->label, dfa_idx, charbuf_idx, i + 1);

			/*move(3, 30);
			printw("%c %d %d", prev_state.state, prev_state.index, prev_state.charbufidx);*/
		}

		// rm rf other lines
	}

	LineState ls = textbuf_requestState(_buf, start_row);

	dfa_idx = ls.index;
	dfa_currstate = dfa_getState(decider, ls.state);
	charbuf_idx = ls.charbufidx;

	// render each row
	//for(int64_t r=start_row;r<textbuf_lineCount(_buf);++r)
	/*move(30, 30);
	printw("%d", (int)_contentDisp->height);*/
	int64_t disp_height = _contentDisp->height;
	if(_contentDisp->height > textbuf_lineCount(_buf) - start_row)
		disp_height = textbuf_lineCount(_buf) - start_row;
	
	for(int64_t r=start_row;r<disp_height + start_row;++r)
	{
		// render the line number
		move(r - _contentDisp->offsetY + 1, 0);
		printw("%d", r + 1);

		uint32_t highlight_color = HIGHLIGHT_NONE;

		// linearly get to the correct state
		//DFAState* dfa_currstate = dfa_getState(decider, 'N');
		//uint32_t charbuf_idx = 0;
		//for(int64_t i=0;i<row_begin + _contentDisp->offsetX;)
		//{
			//uint32_t next_pos = gapBuf_uintAt(_charbuf->charIndex, charbuf_idx);
			//dfa_currstate = dfa_nextState(decider, dfa_currstate, textbuf_charAt(_buf, i));
			//if(i == 0)
				//i += next_pos;
			//else
				//i += next_pos + 1;
			//++ charbuf_idx;
		//}

		while(dfa_idx < row_begin + _contentDisp->offsetX)
		{
			uint32_t next_pos = gapBuf_uintAt(_charbuf->charIndex, charbuf_idx);
			dfa_currstate = dfa_nextState(decider, dfa_currstate, textbuf_charAt(_buf, dfa_idx));

			if(charbuf_idx == 0)
				dfa_idx += next_pos;
			else
				dfa_idx += next_pos + 1;
			++ charbuf_idx;
		}
		//dfa_idx = row_begin;

		// render the content
		for(int64_t i=row_begin + _contentDisp->offsetX;i<textbuf_size(_buf);++i)
		{
			char currlabel = dfa_currstate->label;
			//bool highlight = currstate->label == 'H';
			dfa_currstate = dfa_nextState(decider, dfa_currstate, textbuf_charAt(_buf, i));
			++ dfa_idx;

			// out of bounds
			if(i - _contentDisp->offsetX - row_begin >= _contentDisp->width)
				break;
			if(i - row_begin >= textbuf_rowSize(_buf, r))
				break;
			
			// get the char
			char curr_char = textbuf_charAt(_buf, i);
			chtype display_char = (chtype)curr_char;

			if(curr_char == '/' || curr_char == '"' || curr_char == '\'' || curr_char == '*' || curr_char == '\n')
				++ charbuf_idx;

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
			if(currlabel == 'M' || currlabel == 'S')
				attron(COLOR_PAIR(4));
			if(currlabel == 'D' || currlabel == 'Q')
				attron(COLOR_PAIR(3));
			addch(display_char);
			if(highlight_color == HIGHLIGHT_KEYWORD)
				attroff(COLOR_PAIR(2));
			if(currlabel == 'M' || currlabel == 'S')
				attroff(COLOR_PAIR(4));
			if(currlabel == 'D' || currlabel == 'Q')
				attroff(COLOR_PAIR(3));
		}

		row_begin += (int64_t)textbuf_rowSize(_buf, r) + 1;

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

void disp_render(struct TextBuffer* _buf, struct ContentDisplay* _contentDisp, CharBuffer* _charbuf)
{
	disp_clear();

	disp_renderTopbar();
	disp_renderContent(_buf, _contentDisp, _charbuf);
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
	//if(hash_value == 1134474063039758270) return HIGHLIGHT_KEYWORD;
	if(isKeyword(hash_value) == 1) return HIGHLIGHT_KEYWORD;
	
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
