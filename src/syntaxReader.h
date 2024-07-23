#ifndef WACK_SYNTAXREADER_H
#define WACK_SYNTAXREADER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#include "debugBuffer.h"

extern uint64_t* syntax_keywordHashArr;
extern int32_t syntax_keywordCount;

void syntax_initialize();
void syntax_terminate();

int isKeyword(uint64_t _hash);

#endif
