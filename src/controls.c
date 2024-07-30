#include "controls.h"

StrBuf* ctrl_commands;

void ctrl_init()
{
	ctrl_commands = strbuf_init(128);
}

int processKey(chtype _key, struct TextBuffer* _textbuf, CharBuffer* _charbuf, int* _insert)
{
	if(_key == KEY_LEFT)
	{
		textbuf_cursorLeft(_textbuf);
		charbuf_left(_charbuf);
	} else if(_key == KEY_RIGHT)
	{
		textbuf_cursorRight(_textbuf);
		charbuf_right(_charbuf);
	} else if(_key == KEY_UP)
	{
		textbuf_cursorUp(_textbuf);
		charbuf_up(_charbuf, textbuf_cursorPos(_textbuf));
	} else if(_key == KEY_DOWN)
	{
		textbuf_cursorDown(_textbuf);
		charbuf_down(_charbuf, textbuf_cursorPos(_textbuf));
	}

	if(*_insert == 0)
		ctrl_execCommandMode(_key, _textbuf, _charbuf);
	else
		ctrl_execInsertMode(_key, _textbuf, _charbuf);

	if(_key == 27)
	{
		if(*_insert == 0) *_insert = 1;
		else if(*_insert == 1) *_insert = 0;
	}

	debug_write(ctrl_commands->buffer, strbuf_size(ctrl_commands));
	debug_write("\n", 1);

	if(_key == '|') return 0;

	return 1;
}

void ctrl_execInsertMode(chtype _key, struct TextBuffer* _textbuf, CharBuffer* _charbuf)
{
	if((_key >= ' ' && _key <= '~') || _key == '\n')
	{
		textbuf_addChar(_textbuf, (char)_key);
		charbuf_insert(_charbuf, (char)_key);
	} else if(_key == KEY_BACKSPACE)
	{
		charbuf_remove(_charbuf, textbuf_charAt(_textbuf, textbuf_cursorPos(_textbuf)));
		textbuf_removeChar(_textbuf);
	} else if(_key == 9)
	{
		for(uint32_t i=0;i<4;++i)
		{
			textbuf_addChar(_textbuf, ' ');
			charbuf_insert(_charbuf, ' ');
		}
	}
}

void ctrl_execCommandMode(chtype _key, struct TextBuffer* _textbuf, CharBuffer* _charbuf)
{
	if(_key == ':' || strbuf_size(ctrl_commands) > 0)
	{
		if(_key == KEY_BACKSPACE)
		{
			strbuf_remove(ctrl_commands, 1);
			return;
		}
		if(_key == '\n')
		{
			strbuf_clear(ctrl_commands);
			return;
		}
		// remember to restrict to only displayable chars
		strbuf_add(ctrl_commands, (char)_key);
	}

	if(_key == 'h')
	{
		textbuf_cursorLeft(_textbuf);
		charbuf_left(_charbuf);
	} else if(_key == 'l')
	{
		textbuf_cursorRight(_textbuf);
		charbuf_right(_charbuf);
	} else if(_key == 'k')
	{
		textbuf_cursorUp(_textbuf);
		charbuf_up(_charbuf, textbuf_cursorPos(_textbuf));
	} else if(_key == 'j')
	{
		textbuf_cursorDown(_textbuf);
		charbuf_down(_charbuf, textbuf_cursorPos(_textbuf));
	}
}

void ctrl_free()
{
	strbuf_free(ctrl_commands);
}
