#ifndef WACK_CHARBUF_H
#define WACK_CHARBUF_H

#include <stdlib.h>

#include "gapBuffer.h"
#include "debugBuffer.h"

typedef struct
{
	uint32_t idx;
	uint32_t dist; // keep track of current distance
	uint32_t size;
	char* specialChars;
	uint32_t specialCharSize;
	struct GapBuffer* charBuffer;
	struct GapBuffer* charIndex;
} CharBuffer;

typedef struct
{
	char character;
	uint32_t distance;
} CharDistance;

// parameter for starting size?
CharBuffer* charbuf_create();
void charbuf_setSpecialChars(CharBuffer* _buf, char* _characters, uint32_t _size);
void charbuf_free(CharBuffer* _buf);

void charbuf_insert(CharBuffer* _buf, char _c); // this function have everything to calculate distance itself
void charbuf_remove(CharBuffer* _buf, char _c); // can determine if the char should be removed

// user do their own checks since buffer is uninformed of textbuffer size
void charbuf_up(CharBuffer* _buf, uint32_t _newidx);
void charbuf_down(CharBuffer* _buf, uint32_t _newidx);
void charbuf_left(CharBuffer* _buf);
void charbuf_right(CharBuffer* _buf);

// returns distance to and char
void charbuf_nextChar(CharDistance* res, CharBuffer* _buf, uint32_t _relativeIdx);

#endif
