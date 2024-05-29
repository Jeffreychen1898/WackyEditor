#ifndef WACK_DEBUGBUF_H
#define WACK_DEBUGBUF_H

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

extern char* debug_text;
extern uint32_t debug_bufferSize;
extern uint32_t debug_currIdx;

void debug_init(uint32_t _initialSize);
void debug_terminate();

void debug_write(char* _c, uint32_t _count);

void debug_resize(uint32_t _newSize);

inline void debug_clear() { debug_currIdx = 0; };

inline char debug_getChar(uint32_t _idx)
{
	if(debug_text == NULL) return '\0';
	if(_idx >= debug_currIdx) return '\0';

	return debug_text[_idx];
}

inline uint32_t debug_strlen() { return debug_currIdx; };

#endif
