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

struct TextBuffer* textbuf_init(uint32_t _charBufCapacity, uint32_t _lineCapacity);
void textbuf_free(struct TextBuffer* _buf);

void textbuf_addChar(struct TextBuffer* _buf, char _c);
void textbuf_removeChar(struct TextBuffer* _buf);

void textbuf_cursorUp(struct TextBuffer* _buf);
void textbuf_cursorDown(struct TextBuffer* _buf);
void textbuf_cursorLeft(struct TextBuffer* _buf);
void textbuf_cursorRight(struct TextBuffer* _buf);

char textbuf_charAt(struct TextBuffer* _buf, uint32_t _idx);
uint32_t textbuf_size(struct TextBuffer* _buf);
uint32_t textbuf_lineCount(struct TextBuffer* _buf);
uint32_t textbuf_cursorPos(struct TextBuffer* _buf);
inline uint32_t textbuf_col(const struct TextBuffer* _buf) { return _buf->currCol; };
inline uint32_t textbuf_row(const struct TextBuffer* _buf) { return _buf->currRow; };

inline uint32_t textbuf_rowSize(struct TextBuffer* _buf, uint32_t _row)
{
	return gapBuf_uintAt(_buf->lineColumns, _row);
};

#endif
