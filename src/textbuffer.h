#ifndef WACK_TEXTBUFFER_H
#define WACK_TEXTBUFFER_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "gapBuffer.h"

struct TextBuffer
{
	uint32_t currRow;
	uint32_t currCol;

	struct GapBuffer* buffer;
	struct GapBuffer* lineColumns;
};

// creating and deleting the buffer
struct TextBuffer* textbuf_init(uint32_t _charBufCapacity, uint32_t _lineCapacity);
void textbuf_free(struct TextBuffer* _buf);

// add and remove character
void textbuf_addChar(struct TextBuffer* _buf, char _c);
void textbuf_removeChar(struct TextBuffer* _buf);

// move cursor around
void textbuf_cursorUp(struct TextBuffer* _buf);
void textbuf_cursorDown(struct TextBuffer* _buf);
void textbuf_cursorLeft(struct TextBuffer* _buf);
void textbuf_cursorRight(struct TextBuffer* _buf);

// get and set values
inline char textbuf_charAt(struct TextBuffer* _buf, uint32_t _idx)
{
	if(_buf == NULL || _buf->buffer == NULL) return '\0';

	return gapBuf_charAt(_buf->buffer, _idx);
}

inline uint32_t textbuf_size(struct TextBuffer* _buf)
{
	if(_buf == NULL || _buf->buffer == NULL) return 0;

	return gapBuf_size(_buf->buffer);
}

inline uint32_t textbuf_lineCount(struct TextBuffer* _buf)
{
	if(_buf == NULL) return 0;

	return gapBuf_size(_buf->lineColumns);
}

inline uint32_t textbuf_cursorPos(struct TextBuffer* _buf)
{
	if(_buf == NULL) return 0;

	return gapBuf_gapStart(_buf->buffer);
}

inline uint32_t textbuf_col(const struct TextBuffer* _buf)
{
	if(_buf == NULL) return 0;

	return _buf->currCol;
}

inline uint32_t textbuf_row(const struct TextBuffer* _buf)
{
	if(_buf == NULL) return 0;

	return _buf->currRow;
}

inline uint32_t textbuf_rowSize(struct TextBuffer* _buf, uint32_t _row)
{
	if(_buf == NULL) return 0;

	return gapBuf_uintAt(_buf->lineColumns, _row);
}

#endif
