#include "charbuffer.h"

static int charbuf_compareChars(const void* _a, const void* _b)
{
	// return a - b
	return *((char*)_a) - *((char*)_b);
}

static int charbuf_isSpecial(char* _specialChars, uint32_t _size, char _c)
{
	int64_t l = 0;
	int64_t r = _size - 1;

	while(l <= r)
	{
		int64_t m = (l + r) / 2;

		if(_specialChars[m] == _c)
			return 1; // found it

		else if(_specialChars[m] > _c)
			r = m - 1;

		else if(_specialChars[m] < _c)
			l = m + 1;
	}

	return 0;
}

CharBuffer* charbuf_create()
{
	CharBuffer* new_buffer = (CharBuffer*)malloc(sizeof(CharBuffer));

	new_buffer->charBuffer = gapBuf_createCharBuf(5);
	new_buffer->charIndex = gapBuf_createUintBuf(5);

	new_buffer->idx = 0;
	new_buffer->dist = 1;
	new_buffer->size = 0;

	new_buffer->specialChars = NULL;
	new_buffer->specialCharSize = 0;

	gapBuf_insertChar(new_buffer->charBuffer, '*');
	gapBuf_insertUint(new_buffer->charIndex, 0);

	gapBuf_moveLeft(new_buffer->charBuffer, 1);
	gapBuf_moveLeft(new_buffer->charIndex, 1);

	return new_buffer;
}

void charbuf_setSpecialChars(CharBuffer* _buf, char* _characters, uint32_t _size)
{
	if(_buf->specialChars != NULL)
		free(_buf->specialChars);

	char* copy_buffer = (char*)malloc(sizeof(char) * _size);
	memcpy(copy_buffer, _characters, sizeof(char) * _size);

	qsort(copy_buffer, _size, sizeof(char), charbuf_compareChars);

	_buf->specialChars = copy_buffer;
	_buf->specialCharSize = _size;
}

// this function have everything to calculate distance itself
void charbuf_insert(CharBuffer* _buf, char _c)
{
	if(_buf->specialChars == NULL) return;
	++ _buf->idx;
	++ _buf->size;

	int is_special = charbuf_isSpecial(_buf->specialChars, _buf->specialCharSize, _c);
	//int is_special = _c == '*';

	uint32_t next_index = gapBuf_gapStart(_buf->charIndex);
	uint32_t get_distance = gapBuf_uintAt(_buf->charIndex, next_index);
	// only store special characters
	if(is_special == 0)
	{
		if(_buf->dist == 0)
		{
			get_distance = gapBuf_uintAt(_buf->charIndex, next_index - 1);
			gapBuf_setUint(_buf->charIndex, next_index - 1, get_distance + 1);
		}
		else
		{
			gapBuf_setUint(_buf->charIndex, next_index, get_distance + 1);
			++ _buf->dist;
		}

		return;
	}

	if(_buf->dist == 0)
	{
		get_distance = gapBuf_uintAt(_buf->charIndex, next_index - 1);

		gapBuf_insertChar(_buf->charBuffer, _c);
		gapBuf_insertUint(_buf->charIndex, get_distance);

		gapBuf_setUint(_buf->charIndex, next_index, 0);

		_buf->dist = 0;
		return;
	}
	// update the next relative distance
	gapBuf_setUint(_buf->charIndex, next_index, get_distance - _buf->dist + 1);

	// insert the characters
	gapBuf_insertChar(_buf->charBuffer, _c);
	gapBuf_insertUint(_buf->charIndex, _buf->dist - 1);

	// reset the distance
	_buf->dist = 1;
}

// can determine if the char should be removed
void charbuf_remove(CharBuffer* _buf, char _c)
{
	if(_buf->idx == 0)
		return;
	if(_buf->specialChars == NULL) return;

	//-- _buf->size;

	uint32_t next_index = gapBuf_gapStart(_buf->charIndex);
	if(_buf->dist > 1) // not a special character
	{
		// update the next special character relative position
		gapBuf_setUint(_buf->charIndex, next_index, gapBuf_uintAt(_buf->charIndex, next_index) - 1);
		// the cursor is one character to the left
		-- _buf->dist;
		-- _buf->idx;
		return;
	}

	if(_buf->dist == 1)
	{
		-- _buf->idx;
		_buf->dist = gapBuf_currUint(_buf->charIndex) + 1;

		// remove the character
		gapBuf_remove(_buf->charBuffer, 1);
		gapBuf_remove(_buf->charIndex, 1);

		// update the distance of next char
		next_index = gapBuf_gapStart(_buf->charIndex);
		uint32_t next_distance = gapBuf_uintAt(_buf->charIndex, next_index);
		gapBuf_setUint(_buf->charIndex, next_index, next_distance + _buf->dist - 1);

		return;
	}

	uint32_t curr_dist = gapBuf_currUint(_buf->charIndex);
	if(curr_dist > 0)
	{
		gapBuf_setUint(_buf->charIndex, next_index - 1, curr_dist - 1);
		-- _buf->idx;

		return;
	}

	uint32_t prev_dist = gapBuf_uintAt(_buf->charIndex, next_index - 2);
	gapBuf_setUint(_buf->charIndex, next_index - 1, curr_dist + prev_dist);
	-- _buf->idx;
	gapBuf_moveLeft(_buf->charBuffer, 1);
	gapBuf_moveLeft(_buf->charIndex, 1);

	gapBuf_remove(_buf->charBuffer, 1);
	gapBuf_remove(_buf->charIndex, 1);

	gapBuf_moveRight(_buf->charBuffer, 1);
	gapBuf_moveRight(_buf->charIndex, 1);
}

void charbuf_up(CharBuffer* _buf, uint32_t _newidx)
{
	if(_buf->specialChars == NULL) return;
	uint32_t move_amount = _buf->idx - _newidx;

	// if no traversals are needed
	if(move_amount <= _buf->dist)
	{
		_buf->idx = _newidx;
		_buf->dist -= move_amount;
		return;
	}

	_buf->idx = _newidx;
	// move to closest special char
	int64_t amount_left = move_amount - _buf->dist;

	while(1)
	{
		// gap buf move left
		uint32_t get_distance = gapBuf_currUint(_buf->charIndex) + 1;
		if(amount_left <= get_distance)
		{
			_buf->dist = get_distance - amount_left;
			gapBuf_moveLeft(_buf->charBuffer, 1);
			gapBuf_moveLeft(_buf->charIndex, 1);
			break;
		}

		amount_left -= get_distance;
		gapBuf_moveLeft(_buf->charBuffer, 1);
		gapBuf_moveLeft(_buf->charIndex, 1);
	}
}

void charbuf_down(CharBuffer* _buf, uint32_t _newidx)
{
	if(_buf->specialChars == NULL) return;
	uint32_t move_amount = _newidx - _buf->idx;

	// if no traversals are needed
	uint32_t next_index = gapBuf_gapStart(_buf->charIndex);
	if(move_amount + _buf->dist <= gapBuf_uintAt(_buf->charIndex, next_index))
	{
		_buf->idx = _newidx;
		_buf->dist += move_amount;
		return;
	}

	_buf->idx = _newidx;
	int64_t amount_left = move_amount + _buf->dist - 1;
	amount_left -= gapBuf_uintAt(_buf->charIndex, next_index);
	gapBuf_moveRight(_buf->charBuffer, 1);
	gapBuf_moveRight(_buf->charIndex, 1);
	while(1)
	{
		//gap buf move right
		uint32_t next_index = gapBuf_gapStart(_buf->charIndex);
		uint32_t get_distance = gapBuf_uintAt(_buf->charIndex, next_index);
		if(amount_left <= get_distance)
		{
			//_buf->dist = get_distance - amount_left;
			_buf->dist = amount_left;
			break;
		}

		amount_left -= (get_distance + 1);
		gapBuf_moveRight(_buf->charBuffer, 1);
		gapBuf_moveRight(_buf->charIndex, 1);
	}
}

void charbuf_left(CharBuffer* _buf)
{
	if(_buf->specialChars == NULL) return;
	if(_buf->idx == 0)
		return;

	// update absolute position
	-- _buf->idx;

	// if already at special character, update distance and move left
	if(_buf->dist == 0)
	{
		_buf->dist = gapBuf_currUint(_buf->charIndex);
		gapBuf_moveLeft(_buf->charBuffer, 1);
		gapBuf_moveLeft(_buf->charIndex, 1);
		
		return;
	}

	// else, just update distance
	-- _buf->dist;
}

void charbuf_right(CharBuffer* _buf)
{
	if(_buf->specialChars == NULL) return;
	if(_buf->idx == _buf->size)
		return;

	// update distance and absolute position
	++ _buf->idx;
	++ _buf->dist;

	// if we land on a new special char
	uint32_t next_index = gapBuf_gapStart(_buf->charIndex);
	if(gapBuf_uintAt(_buf->charIndex, next_index) < _buf->dist)
	{
		_buf->dist = 0;
		gapBuf_moveRight(_buf->charBuffer, 1);
		gapBuf_moveRight(_buf->charIndex, 1);

		return;
	}
}

void charbuf_nextChar(CharDistance* res, CharBuffer* _buf, uint32_t _relativeIdx)
{
	if(_buf->specialChars == NULL) return;
	uint32_t next_index = gapBuf_gapStart(_buf->charIndex);

	uint32_t distance_to = gapBuf_uintAt(_buf->charIndex, next_index) - _buf->dist;
	char next_char = gapBuf_charAt(_buf->charBuffer, next_index);

	res->character = next_char;
	res->distance = distance_to;
}

void charbuf_free(CharBuffer* _buf)
{
	if(_buf == NULL) return;

	if(_buf->specialChars != NULL)
		free(_buf->specialChars);

	gapBuf_free(_buf->charBuffer);
	gapBuf_free(_buf->charIndex);

	free(_buf);
}
