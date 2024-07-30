#include "strbuf.h"

StrBuf* strbuf_init(uint32_t _bufsize)
{
	StrBuf* new_buffer = (StrBuf*)malloc(sizeof(StrBuf));
	new_buffer->buffer = (char*)malloc(sizeof(char) * _bufsize);
	new_buffer->lastptr = 0;
	new_buffer->capacity = _bufsize;

	return new_buffer;
}

void strbuf_add(StrBuf* _buf, char _c)
{
	if(_buf == NULL) return;

	// exceeds capacity, need to regrow
	if(_buf->lastptr >= _buf->capacity)
		return;

	_buf->buffer[_buf->lastptr] = _c;
	++ _buf->lastptr;
}

void strbuf_remove(StrBuf* _buf, uint32_t _count)
{
	if(_buf == NULL) return;

	if(_buf->lastptr < _count)
	{
		_buf->lastptr = 0;
		return;
	}

	_buf->lastptr -= _count;
}

void strbuf_clear(StrBuf* _buf)
{
	if(_buf == NULL) return;

	_buf->lastptr = 0;
}

void strbuf_free(StrBuf* _buf)
{
	if(_buf == NULL) return;

	free(_buf->buffer);
	free(_buf);
}
