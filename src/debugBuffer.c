#include "debugBuffer.h"

char* debug_text = NULL;
uint32_t debug_bufferSize = 0;
uint32_t debug_currIdx = 0;

void debug_init(uint32_t _initialSize)
{
	if(debug_text != NULL)
		free(debug_text);

	debug_text = malloc(_initialSize * sizeof(char));
	debug_bufferSize = _initialSize;
}

void debug_write(char* _c, uint32_t _count)
{
	if(debug_text == NULL) return;

	while(debug_currIdx + _count > debug_bufferSize)
		debug_resize(debug_bufferSize * 2);

	memcpy(debug_text + debug_currIdx, _c, sizeof(char) * _count);
	debug_currIdx += _count;
}

void debug_resize(uint32_t _newSize)
{
	if(debug_text == NULL) return;

	// reallocate the buffer
	debug_text = (char*)realloc(debug_text, sizeof(char) * _newSize);
	debug_bufferSize = _newSize;
}

void debug_terminate()
{
	if(debug_text == NULL)
		return;

	free(debug_text);
}
