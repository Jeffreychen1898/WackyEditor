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

	if(strbuf_size(ctrl_commands) == 0)
	{
		if(_key == 27 && *_insert == 1)
		{
			*_insert = 0;
			return 1;
		}
		else if(_key == 'i' && *_insert == 0)
		{
			*_insert = 1;
			return 1;
		}
	}

	if(*_insert == 0)
	{
		if(ctrl_execCommandMode(_key, _textbuf, _charbuf) == 1)
			return 0;
	} else
		ctrl_execInsertMode(_key, _textbuf, _charbuf);

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

int ctrl_execCommandMode(chtype _key, struct TextBuffer* _textbuf, CharBuffer* _charbuf)
{
	if(_key == ':' || strbuf_size(ctrl_commands) > 0)
	{
		if(_key == KEY_BACKSPACE)
		{
			strbuf_remove(ctrl_commands, 1);
			return 0;
		}
		if(_key == '\n')
		{
			if(strcmp(ctrl_commands->buffer, ":q") == 0)
				return 1;
			strbuf_clear(ctrl_commands);
			return 0;
		}
		// remember to restrict to only displayable chars
		strbuf_add(ctrl_commands, (char)_key);
		return 0;
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

	return 0;
}

void ctrl_free()
{
	strbuf_free(ctrl_commands);
}
