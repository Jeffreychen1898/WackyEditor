#ifndef CONTROLS_H
#define CONTROLS_H
#include <ncurses.h>

#include "textbuffer.h"
#include "syntaxReader.h"
#include "charbuffer.h"

int processKey(chtype _key, struct TextBuffer* _textbuf, CharBuffer* _charbuf);

#endif
