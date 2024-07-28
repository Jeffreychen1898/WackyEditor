#include "textbuffer.h"

DFADecider* textbuf_decider = NULL;
DFADecider* textbuf_invdecider = NULL;

struct TextBuffer* textbuf_init(uint32_t _charBufCapacity, uint32_t _lineCapacity)
{
	struct TextBuffer* new_buffer = (struct TextBuffer*)malloc(sizeof(struct TextBuffer));

	new_buffer->currRow = 0;
	new_buffer->currCol = 0;

	new_buffer->buffer = gapBuf_createCharBuf(_charBufCapacity);
	new_buffer->lineColumns = gapBuf_createUintBuf(_lineCapacity);
	new_buffer->lineStates = gapBuf_createCharBuf(_lineCapacity);
	new_buffer->lineIndex = gapBuf_createUintBuf(_lineCapacity);
	new_buffer->charbufIdx = gapBuf_createUintBuf(_lineCapacity);

	gapBuf_insertUint(new_buffer->lineColumns, 0);

	return new_buffer;
}

void textbuf_addChar(struct TextBuffer* _buf, char _c)
{
	if(_buf == NULL) return;

	gapBuf_insertChar(_buf->buffer, _c);

	// update the row and column book keeping
	uint32_t curr_line_cols = gapBuf_currUint(_buf->lineColumns);

	// if a new line is created
	if(_c == '\n')
	{
		uint32_t remaining_amount = curr_line_cols - _buf->currCol;
		gapBuf_setUint(_buf->lineColumns, _buf->currRow, curr_line_cols - remaining_amount);
		if((uint32_t)(curr_line_cols - remaining_amount) > 500)
			exit(4);
		_buf->currCol = 0;
		++ _buf->currRow;
		gapBuf_insertUint(_buf->lineColumns, remaining_amount);
	}
	else { // if a normal char is added
		gapBuf_setUint(_buf->lineColumns, _buf->currRow, curr_line_cols + 1);
		if((uint32_t)curr_line_cols > 500)
			exit(4);
		++ _buf->currCol;
	}
}

void textbuf_removeChar(struct TextBuffer* _buf)
{
	// if nothing in the buffer, can't delete anything
	if(_buf == NULL || gapBuf_gapStart(_buf->buffer) == 0) return;

	char remove_char = gapBuf_currChar(_buf->buffer);

	if(remove_char == '\n')
	{
		uint32_t lines_left = gapBuf_currUint(_buf->lineColumns) - _buf->currCol;
		gapBuf_remove(_buf->lineColumns, 1);
		-- _buf->currRow;
		uint32_t prev_line_count = gapBuf_currUint(_buf->lineColumns);
		_buf->currCol = prev_line_count;
		gapBuf_setUint(_buf->lineColumns, _buf->currRow, prev_line_count + lines_left);
		if((uint32_t)(prev_line_count + lines_left) > 1000)
			exit(4);
	}
	else
	{
		-- _buf->currCol;
		uint32_t curr_line_count = gapBuf_currUint(_buf->lineColumns);
		gapBuf_setUint(_buf->lineColumns, _buf->currRow, curr_line_count - 1);
		if((uint32_t)(curr_line_count - 1) > 1000)
			exit(4);
	}

	// removing the char
	gapBuf_remove(_buf->buffer, 1);
}

LineState textbuf_requestState(struct TextBuffer* _buf, uint32_t _line)
{
	uint32_t buf_size = gapBuf_size(_buf->lineStates);
	if(buf_size == 0)
	{
		LineState result;
		result.state = '\0';
		result.index = 0;
		result.charbufidx = 0;
		return result;
	}

	if(_line >= buf_size)
	{
		// not enough lines
		uint32_t line_index = gapBuf_currUint(_buf->lineIndex);
		uint32_t line_state = gapBuf_currChar(_buf->lineStates);

		LineState result;
		result.state = line_state;
		result.index = line_index;
		result.charbufidx = gapBuf_currUint(_buf->charbufIdx);
		return result;
	}

	uint32_t idx = gapBuf_uintAt(_buf->lineIndex, _line);
	char state = gapBuf_charAt(_buf->lineStates, _line);

	LineState result;
	result.state = state;
	result.index = idx;
	result.charbufidx = gapBuf_uintAt(_buf->charbufIdx, _line);
	return result;
}

void textbuf_insertState(struct TextBuffer* _buf, char _state, uint32_t _idx, uint32_t _charbufidx)
{
	gapBuf_insertUint(_buf->lineIndex, _idx);
	gapBuf_insertChar(_buf->lineStates, _state);
	gapBuf_insertUint(_buf->charbufIdx, _charbufidx);
}

void textbuf_removeState(struct TextBuffer* _buf, uint32_t _line)
{
	while(gapBuf_size(_buf->lineIndex) > _line)
	{
		gapBuf_remove(_buf->lineIndex, 1);
		gapBuf_remove(_buf->lineStates, 1);
		gapBuf_remove(_buf->charbufIdx, 1);
	}
}

void textbuf_cursorUp(struct TextBuffer* _buf)
{
	if(_buf == NULL || _buf->currRow == 0) return;

	-- _buf->currRow;
	gapBuf_moveLeft(_buf->lineColumns, 1);

	uint32_t prev_line_size = gapBuf_currUint(_buf->lineColumns);
	if(_buf->currCol > prev_line_size)
	{
		gapBuf_moveLeft(_buf->buffer, _buf->currCol + 1);
		_buf->currCol = prev_line_size;
	} else
	{
		gapBuf_moveLeft(_buf->buffer, prev_line_size + 1);
	}
}

void textbuf_cursorDown(struct TextBuffer* _buf)
{
	if(_buf == NULL) return;
	if(_buf->currRow >= gapBuf_size(_buf->lineColumns) - 1) return;

	uint32_t curr_line_size = gapBuf_currUint(_buf->lineColumns);

	++ _buf->currRow;
	gapBuf_moveRight(_buf->lineColumns, 1);

	uint32_t next_line_size = gapBuf_currUint(_buf->lineColumns);
	uint32_t curr_line_remaining = curr_line_size - _buf->currCol;
	if(_buf->currCol > next_line_size)
	{
		gapBuf_moveRight(_buf->buffer, next_line_size + curr_line_remaining + 1);
		_buf->currCol = next_line_size;
	} else
	{
		gapBuf_moveRight(_buf->buffer, curr_line_size + 1);
	}
}

void textbuf_cursorLeft(struct TextBuffer* _buf)
{
	if(_buf == NULL) return;
	if(gapBuf_gapStart(_buf->buffer) == 0)
		return;

	if(gapBuf_currChar(_buf->buffer) == '\n')
	{
		-- _buf->currRow;
		gapBuf_moveLeft(_buf->lineColumns, 1);
		_buf->currCol = gapBuf_currUint(_buf->lineColumns);
	} else
		-- _buf->currCol;

	gapBuf_moveLeft(_buf->buffer, 1);
}

void textbuf_cursorRight(struct TextBuffer* _buf)
{
	if(_buf == NULL) return;
	if(gapBuf_gapStart(_buf->buffer) >= gapBuf_size(_buf->buffer))
		return;

	uint32_t curr_index = gapBuf_gapStart(_buf->buffer);
	if(gapBuf_charAt(_buf->buffer, curr_index) == '\n')
	{
		++ _buf->currRow;
		_buf->currCol = 0;
		gapBuf_moveRight(_buf->lineColumns, 1);
	} else
		++ _buf->currCol;

	gapBuf_moveRight(_buf->buffer, 1);
}

void textbuf_free(struct TextBuffer* _buf)
{
	if(_buf == NULL) return;

	gapBuf_free(_buf->buffer);
	gapBuf_free(_buf->lineColumns);

	gapBuf_free(_buf->lineStates);
	gapBuf_free(_buf->lineIndex);
	gapBuf_free(_buf->charbufIdx);

	free(_buf);
}
