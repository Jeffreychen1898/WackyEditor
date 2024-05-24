#include "gapBuffer.h"

struct GapBuffer* gapBuf_createCharBuf(uint32_t _capacity)
{
	struct GapBuffer* new_buffer = malloc(sizeof(struct GapBuffer));
	new_buffer->gapStart = 0;
	new_buffer->gapSize = _capacity;
	new_buffer->initialGapSize = _capacity;
	new_buffer->size = 0;

	// create the buffer
	new_buffer->charBuffer = (char*)malloc(sizeof(char) * _capacity);
	new_buffer->uintBuffer = NULL;

	return new_buffer;
}

struct GapBuffer* gapBuf_createUintBuf(uint32_t _capacity)
{
	struct GapBuffer* new_buffer = malloc(sizeof(struct GapBuffer));
	new_buffer->gapStart = 0;
	new_buffer->gapSize = _capacity;
	new_buffer->initialGapSize = _capacity;
	new_buffer->size = 0;

	// create the buffer
	new_buffer->charBuffer = NULL;
	new_buffer->uintBuffer = (uint32_t*)malloc(sizeof(uint32_t) * _capacity);

	return new_buffer;
}

void gapBuf_moveLeft(struct GapBuffer* _buf, uint32_t _steps)
{
	// make sure it does not go out of bounds
	int64_t difference = _buf->gapStart - _steps;
	if((int64_t)_buf->gapStart - (int64_t)_steps < 0)
		_steps = _buf->gapStart;

	// move the gap to the left
	uint32_t dst_index = _buf->gapStart + _buf->gapSize - _steps;
	uint32_t src_index = _buf->gapStart - _steps;

	if(_buf->charBuffer != NULL)
	{
		memcpy(
			_buf->charBuffer + dst_index,
			_buf->charBuffer + src_index,
			sizeof(char) * _steps);
	}
	else if(_buf->uintBuffer != NULL)
	{
		memcpy(
			_buf->uintBuffer + dst_index,
			_buf->uintBuffer + src_index,
			sizeof(char) * _steps);
	}

	// update the gap start position
	_buf->gapStart -= _steps;
}

void gapBuf_moveRight(struct GapBuffer* _buf, uint32_t _steps)
{
	// make sure it does not go out of bounds
	if(_buf->gapStart + _steps > _buf->size)
		_steps = _buf->size - _buf->gapStart;

	// move the gap to the right
	if(_buf->charBuffer != NULL)
	{
		memcpy(
			_buf->charBuffer + _buf->gapStart,
			_buf->charBuffer + _buf->gapStart + _buf->gapSize,
			sizeof(char) * _steps);
	}
	else if(_buf->uintBuffer != NULL) {
		memcpy(
			_buf->uintBuffer + _buf->gapStart,
			_buf->uintBuffer + _buf->gapStart + _buf->gapSize,
			sizeof(uint32_t) * _steps);
	}

	// update the gap start position
	_buf->gapStart += _steps;
}

void gapBuf_insertChar(struct GapBuffer* _buf, char _c)
{
	// make sure the buffer exist
	if(_buf->charBuffer == NULL) return;

	// double gap size if ran out of space
	if(_buf->gapSize <= 0)
		gapBuf_grow(_buf, _buf->initialGapSize * 2);

	// update the gap buffer
	_buf->charBuffer[_buf->gapStart] = _c;
	++ _buf->gapStart;
	-- _buf->gapSize;
	++ _buf->size;
}

void gapBuf_insertUint(struct GapBuffer* _buf, uint32_t _v)
{
	// make sure the buffer exist
	if(_buf->uintBuffer == NULL) return;

	// double the gap size if ran out of space
	if(_buf->gapSize <= 0)
		gapBuf_grow(_buf, _buf->initialGapSize * 2);

	// update the gap buffer
	_buf->uintBuffer[_buf->gapStart] = _v;
	++ _buf->gapStart;
	-- _buf->gapSize;
	++ _buf->size;
}

char gapBuf_currChar(struct GapBuffer* _buf)
{
	if(_buf->size == 0) return '\0';

	return _buf->charBuffer[_buf->gapStart - 1];
}

uint32_t gapBuf_currUint(struct GapBuffer* _buf)
{
	if(_buf->size == 0) return 0;

	return _buf->uintBuffer[_buf->gapStart - 1];
}

char gapBuf_charAt(struct GapBuffer* _buf, uint32_t _idx)
{
	// error checking
	if(_buf->charBuffer == NULL) return '\0';
	if(_idx >= _buf->size) return '\0';

	// if on the back side, increase index by size of gap
	if(_idx >= _buf->gapStart)
		_idx += _buf->gapSize;

	return _buf->charBuffer[_idx];
}

uint32_t gapBuf_uintAt(struct GapBuffer* _buf, uint32_t _idx)
{
	// error checking
	if(_buf->uintBuffer == NULL) return 0;
	if(_idx >= _buf->size) return 0;

	// if on the back side, increase index by size of gap
	if(_idx >= _buf->gapStart)
		_idx += _buf->gapSize;

	return _buf->uintBuffer[_idx];
}

void gapBuf_setChar(struct GapBuffer* _buf, uint32_t _idx, char _c)
{
	// error checking
	if(_buf->charBuffer == NULL) return;
	if(_idx >= _buf->size) return;

	// if on the back side, increase index by size of gap
	if(_idx >= _buf->gapStart)
		_idx += _buf->gapSize;

	_buf->charBuffer[_idx] = _c;
}

void gapBuf_setUint(struct GapBuffer* _buf, uint32_t _idx, uint32_t _v)
{
	// error checking
	if(_buf->uintBuffer == NULL) return;
	if(_idx >= _buf->size) return;

	// if on the back side, increase index by size of gap
	if(_idx >= _buf->gapStart)
		_idx += _buf->gapSize;

	_buf->uintBuffer[_idx] = _v;
}

void gapBuf_grow(struct GapBuffer* _buf, uint32_t _gapSize)
{
	// ensure it is ACTUALLY growing
	if(_gapSize <= _buf->gapSize)
		return;

	// some helpful variables
	uint32_t prev_total_size = _buf->gapSize + _buf->size;
	uint32_t new_total_size = _buf->size + _gapSize;
	uint32_t backside_size = prev_total_size - _buf->gapSize - _buf->gapStart;

	// create the new buffer
	if(_buf->charBuffer != NULL)
	{
		_buf->charBuffer = (char*)realloc(_buf->charBuffer, sizeof(char) * new_total_size);
		// shift the back chunk of the array down
		memmove(
			_buf->charBuffer + _buf->gapStart + _gapSize,
			_buf->charBuffer + _buf->gapStart + _buf->gapSize,
			sizeof(char) * backside_size);
	}
	else if(_buf->uintBuffer != NULL)
	{
		_buf->uintBuffer = (uint32_t*)realloc(_buf->uintBuffer, sizeof(uint32_t) * new_total_size);
		// shift the back chunk of the array down
		memmove(
			_buf->uintBuffer + _buf->gapStart + _gapSize,
			_buf->uintBuffer + _buf->gapStart + _buf->gapSize,
			sizeof(uint32_t) * backside_size);
	}

	// update the size variables
	_buf->initialGapSize = _gapSize;
	_buf->gapSize = _gapSize;
}

void gapBuf_remove(struct GapBuffer* _buf, uint32_t _count)
{
	if(_count > _buf->gapStart)
		_count = _buf->gapStart;

	_buf->gapStart -= _count;
	_buf->gapSize += _count;
	_buf->size -= _count;
}

void gapBuf_free(struct GapBuffer* _buf)
{
	if(_buf->charBuffer != NULL)
		free(_buf->charBuffer);

	if(_buf->uintBuffer != NULL)
		free(_buf->uintBuffer);

	free(_buf);
}
