#include "syntaxReader.h"

uint64_t* syntax_keywordHashArr = NULL;
int32_t syntax_keywordCount = 0;

void syntax_initialize()
{
	// open the file
	FILE* syntax_file = fopen("res/keywords.txt", "r");
	if(syntax_file == NULL)
	{
		debug_write("loading fail", 12);
		return;
	}

	// read the number of keywords
	syntax_keywordCount = 0;
	if(fscanf(syntax_file, "%d", &syntax_keywordCount) != 1)
		return;

	// create the buffer to store the hashes
	syntax_keywordHashArr = malloc(sizeof(uint64_t) * syntax_keywordCount);

	// read all the keywords
	char line_buffer[11];
	unsigned long long keyword_hash = 0;
	uint32_t i = 0;
	while(fscanf(syntax_file, "%10s %llu", line_buffer, &keyword_hash) == 2)
	{
		syntax_keywordHashArr[i] = keyword_hash;
		++ i;
	}
}

int isKeyword(uint64_t _hash)
{
	if(syntax_keywordHashArr == NULL) return 0;

	// binary search
	int32_t left = 0;
	int32_t right = syntax_keywordCount - 1;
	while(left <= right)
	{
		int32_t middle = (left + right) / 2;

		if(syntax_keywordHashArr[middle] == _hash) return 1;

		if(syntax_keywordHashArr[middle] > _hash)
			right = middle - 1;
		else
			left = middle + 1;
	}

	return 0;
}

void syntax_terminate()
{
	if(syntax_keywordHashArr == NULL) return;

	free(syntax_keywordHashArr);
}
