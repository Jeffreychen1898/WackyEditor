#include "controls.h"

int processKey(chtype _key, struct TextBuffer* _textbuf, CharBuffer* _charbuf)
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

	if(_key == 27) return 0;

	return 1;
}
