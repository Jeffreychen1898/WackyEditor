#ifndef STRBUF_H
#define STRBUF_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

typedef struct
{
	char* buffer;
	uint32_t lastptr;
	uint32_t capacity;
} StrBuf;

StrBuf* strbuf_init(uint32_t _bufsize);

void strbuf_add(StrBuf* _buf, char _c);
void strbuf_remove(StrBuf* _buf, uint32_t _count);
void strbuf_clear(StrBuf* _buf);

// implement growing later

inline uint32_t strbuf_capacity(StrBuf* _buf)
{
	if(_buf == NULL) return 0;
	return _buf->capacity;
};

inline uint32_t strbuf_size(StrBuf* _buf)
{
	if(_buf == NULL) return 0;
	return _buf->lastptr;
};

void strbuf_free(StrBuf* _buf);

#endif
