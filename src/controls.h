#ifndef CONTROLS_H
#define CONTROLS_H
#include <ncurses.h>
#include <stdio.h>
#include <string.h>

#include "textbuffer.h"
#include "syntaxReader.h"
#include "charbuffer.h"
#include "strbuf.h"
#include "debugBuffer.h"

extern StrBuf* ctrl_commands;

void ctrl_init();
void ctrl_free();

int processKey(chtype _key, struct TextBuffer* _textbuf, CharBuffer* _charbuf, int* _insert);

void ctrl_execInsertMode(chtype _key, struct TextBuffer* _textbuf, CharBuffer* _charbuf);
int ctrl_execCommandMode(chtype _key, struct TextBuffer* _textbuf, CharBuffer* _charbuf);

#endif
