#ifndef WACK_GAPBUFFER_H
#define WACK_GAPBUFFER_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

struct GapBuffer
{
	uint32_t gapStart;		 // start index of the gap
	uint32_t gapSize;		 // size of the gap
	uint32_t initialGapSize; // gap size after creation/grow()
	uint32_t size;			 // size of the buffer (counting only existing elems)

	char* charBuffer;
	uint32_t* uintBuffer;
};

// creating the buffer
struct GapBuffer* gapBuf_createCharBuf(uint32_t _capacity);
struct GapBuffer* gapBuf_createUintBuf(uint32_t _capacity);

// moving the gap left and right
void gapBuf_moveLeft(struct GapBuffer* _buf, uint32_t _steps);
void gapBuf_moveRight(struct GapBuffer* _buf, uint32_t _steps);

// insertion
void gapBuf_insertChar(struct GapBuffer* _buf, char _c);
void gapBuf_insertUint(struct GapBuffer* _buf, uint32_t _v);

// get and set value
char gapBuf_currChar(struct GapBuffer* _buf);
uint32_t gapBuf_currUint(struct GapBuffer* _buf);
char gapBuf_charAt(struct GapBuffer* _buf, uint32_t _idx);
uint32_t gapBuf_uintAt(struct GapBuffer* _buf, uint32_t _idx);

void gapBuf_setChar(struct GapBuffer* _buf, uint32_t _idx, char _c);
void gapBuf_setUint(struct GapBuffer* _buf, uint32_t _idx, uint32_t _v);

inline uint32_t gapBuf_size(const struct GapBuffer* _buf) { return _buf->size; };
inline uint32_t gapBuf_gapStart(const struct GapBuffer* _buf) { return _buf->gapStart; };

// modifying the size
void gapBuf_grow(struct GapBuffer* _buf, uint32_t _gapSize);

// deletion
void gapBuf_remove(struct GapBuffer* _buf, uint32_t _count);

void gapBuf_free(struct GapBuffer* _buf);

#endif
