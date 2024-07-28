#ifndef DFA_H
#define DFA_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define DFA_NOSTATE ~0

#define DFA_ERR_TYPE uint8_t
#define DFA_ERR_SYNTAX 1
#define DFA_SUCCESS 0

// DFATransition struct
typedef struct
{
	char nextChar;
	uint32_t toState;
} DFATransition;

// DFAState struct
typedef struct
{
	char label;
	uint32_t defaultNext;
	uint32_t numTransitions;
	DFATransition* transitions;
} DFAState;

// DFAIdStatePair struct
typedef struct
{
	char state;
	uint32_t id;
} IdStatePair;

// DFADecider (what the user will use most of the time)
typedef struct
{
	uint32_t numStates;
	uint32_t stateLookupBucketSize;
	DFAState* dfa;
	IdStatePair* lookup;
} DFADecider;

extern DFAState* dfa_parser;
extern uint32_t dfa_parserStateCount;

void dfa_init();
void dfa_terminate();

// compile
DFADecider* dfa_compile(const char* _dfastr);

// counting states and transitions
DFA_ERR_TYPE dfa_numStatesAndTransitions(const char* _dfastr, uint32_t* _numStates, uint32_t* _maxTransitions);
extern uint32_t dfa_numStates(const DFADecider* _decider);
extern uint32_t dfa_numTransitions(const DFAState* _state);

// get states
uint32_t dfa_nextStateIdx(DFAState* _currState, char _nextChar);
DFAState* dfa_nextState(const DFADecider* _decider, DFAState* _currState, char _nextChar);
DFAState* dfa_getState(const DFADecider* _decider, char _label);

// hashing
extern uint32_t dfa_hashChar(char _c);

// free memory
void dfa_freeDecider(DFADecider* _decider);
void dfa_freeState(DFAState* _state, uint32_t _count);

// printing, for debugging
void dfa_print(const DFADecider* _decider);

#endif
