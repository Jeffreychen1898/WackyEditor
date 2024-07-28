#include "dfa.h"

DFAState* dfa_parser = NULL;
uint32_t dfa_parserStateCount = 0;

uint32_t dfa_stateIndex(IdStatePair* _container, uint32_t _containerSize, char _label);
void dfa_storeStateLabels(IdStatePair* _container, uint32_t _containerSize, const char* _dfastr);

void dfa_init()
{
	if(dfa_parser != NULL)
		free(dfa_parser);
	/*
	 * state 0: reject
	 * state 1: read state
	 * state 2: has state
	 * state 3: read next state
	 * state 4: has next state
	 * state 5: read transition char
	 * state 6: special char
	*/
	dfa_parser = (DFAState*)malloc(sizeof(DFAState) * 7);
	dfa_parserStateCount = 7;
	// set the default next state
	dfa_parser[0].defaultNext = 0;
	dfa_parser[1].defaultNext = 2;
	dfa_parser[2].defaultNext = 0;
	dfa_parser[3].defaultNext = 4;
	dfa_parser[4].defaultNext = 0;
	dfa_parser[5].defaultNext = 5;
	dfa_parser[6].defaultNext = 5;
	dfa_parser[0].label = '0';
	dfa_parser[1].label = '1';
	dfa_parser[2].label = '2';
	dfa_parser[3].label = '3';
	dfa_parser[4].label = '4';
	dfa_parser[5].label = '5';
	dfa_parser[6].label = '6';

	dfa_parser[0].numTransitions = 0;
	dfa_parser[1].numTransitions = 5;
	dfa_parser[2].numTransitions = 1;
	dfa_parser[3].numTransitions = 5;
	dfa_parser[4].numTransitions = 1;
	dfa_parser[5].numTransitions = 5;
	dfa_parser[6].numTransitions = 0;

	dfa_parser[1].transitions = (DFATransition*)malloc(sizeof(DFATransition) * 5);
	// special chars go to reject
	dfa_parser[1].transitions[0].nextChar = '[';
	dfa_parser[1].transitions[0].toState = 0;
	dfa_parser[1].transitions[1].nextChar = ']';
	dfa_parser[1].transitions[1].toState = 0;
	dfa_parser[1].transitions[2].nextChar = ':';
	dfa_parser[1].transitions[2].toState = 0;
	dfa_parser[1].transitions[3].nextChar = '~';
	dfa_parser[1].transitions[3].toState = 0;
	dfa_parser[1].transitions[4].nextChar = ',';
	dfa_parser[1].transitions[4].toState = 0;

	dfa_parser[2].transitions = (DFATransition*)malloc(sizeof(DFATransition));
	// a [ go to the next state
	dfa_parser[2].transitions[0].nextChar = '[';
	dfa_parser[2].transitions[0].toState = 3;

	dfa_parser[3].transitions = (DFATransition*)malloc(sizeof(DFATransition) * 5);
	// special chars go to reject
	dfa_parser[3].transitions[0].nextChar = '[';
	dfa_parser[3].transitions[0].toState = 0;
	dfa_parser[3].transitions[1].nextChar = ']';
	dfa_parser[3].transitions[1].toState = 0;
	dfa_parser[3].transitions[2].nextChar = ':';
	dfa_parser[3].transitions[2].toState = 0;
	dfa_parser[3].transitions[3].nextChar = '~';
	dfa_parser[3].transitions[3].toState = 0;
	dfa_parser[3].transitions[4].nextChar = ',';
	dfa_parser[3].transitions[4].toState = 0;

	dfa_parser[4].transitions = (DFATransition*)malloc(sizeof(DFATransition));
	// : go to the next char
	dfa_parser[4].transitions[0].nextChar = ':';
	dfa_parser[4].transitions[0].toState = 5;

	dfa_parser[5].transitions = (DFATransition*)malloc(sizeof(DFATransition) * 5);
	// special chars for this case
	dfa_parser[5].transitions[0].nextChar = '~';
	dfa_parser[5].transitions[0].toState = 6;
	dfa_parser[5].transitions[1].nextChar = ']';
	dfa_parser[5].transitions[1].toState = 1;
	dfa_parser[5].transitions[2].nextChar = ',';
	dfa_parser[5].transitions[2].toState = 3;
	dfa_parser[5].transitions[3].nextChar = '[';
	dfa_parser[5].transitions[3].toState = 0;
	dfa_parser[5].transitions[4].nextChar = ':';
	dfa_parser[5].transitions[4].toState = 0;
}

DFADecider* dfa_compile(const char* _dfastr)
{
	// count the number of transitions and states
	uint32_t state_count = 0;
	uint32_t max_transitions = 0;

	if(dfa_numStatesAndTransitions(_dfastr, &state_count, &max_transitions) != DFA_SUCCESS)
		return NULL;

	uint32_t hash_bucket_size = state_count * 2;

	// allocate the memory
	DFADecider* new_decider = (DFADecider*)malloc(sizeof(DFADecider));
	new_decider->numStates = state_count;
	new_decider->stateLookupBucketSize = hash_bucket_size;
	new_decider->dfa = (DFAState*)malloc(sizeof(DFAState) * state_count);

	DFATransition* curr_state_transitions = (DFATransition*)malloc(sizeof(DFATransition) * max_transitions);

	// store the label-idx pairs
	new_decider->lookup = (IdStatePair*)calloc(hash_bucket_size, sizeof(IdStatePair));
	dfa_storeStateLabels(new_decider->lookup, hash_bucket_size, _dfastr);

	// build the dfa
	uint32_t transition_to = 0;
	uint32_t num_transitions = 0;
	state_count = 0; // track the _dfastr states
	uint32_t curr_state = 1;
	uint32_t prev_state = 1;

	for(const char* c = _dfastr;*c != '\0';++c)
	{
		uint32_t next_state = dfa_nextStateIdx(&dfa_parser[curr_state], *c);

		if(curr_state == 1 && next_state == 2)
		{
			new_decider->dfa[state_count].label = *c;
		}
		if(curr_state == 3 && next_state == 4)
		{
			// this is the next state
			transition_to = dfa_stateIndex(new_decider->lookup, hash_bucket_size, *c);
		}
		if(curr_state == 5 && (next_state == 3 || next_state == 1))
		{
			if(prev_state == 4)
			{
				// default transition
				new_decider->dfa[state_count].defaultNext = transition_to;
			}
		}
		if((curr_state == 5 && next_state == 5) || (curr_state == 6 && next_state == 5))
		{
			// push the transition
			curr_state_transitions[num_transitions].nextChar = *c;
			curr_state_transitions[num_transitions].toState = transition_to;
			++ num_transitions;
		}
		if(curr_state == 5 && next_state == 1)
		{
			// push the state
			new_decider->dfa[state_count].numTransitions = num_transitions;
			new_decider->dfa[state_count].transitions = (DFATransition*)malloc(sizeof(DFATransition) * num_transitions);
			memcpy(new_decider->dfa[state_count].transitions, curr_state_transitions, sizeof(DFATransition) * num_transitions);
			++ state_count;
			num_transitions = 0;
		}

		prev_state = curr_state;
		curr_state = next_state;
	}

	free(curr_state_transitions);

	return new_decider;
}

void dfa_storeStateLabels(IdStatePair* _container, uint32_t _containerSize, const char* _dfastr)
{
	if(_container == NULL) return;

	uint32_t curr_state = 1;
	uint32_t id_counter = 0;

	for(const char* c = _dfastr;*c != '\0';++c)
	{
		uint32_t next_state = dfa_nextStateIdx(&dfa_parser[curr_state], *c);

		if(curr_state == 1 && next_state == 2)
		{
			uint32_t idx = dfa_hashChar(*c) % _containerSize;
			while(_container[idx].state != '\0')
				idx = (idx + 1) % _containerSize;

			_container[idx].state = *c;
			_container[idx].id = id_counter;
			++ id_counter;
		}

		curr_state = next_state;
	}
}

DFA_ERR_TYPE dfa_numStatesAndTransitions(const char* _dfastr, uint32_t* _numStates, uint32_t* _maxTransitions)
{
	*_numStates = 0;
	*_maxTransitions = 0;
	uint32_t curr_state = 1;
	uint32_t transitions_counter = 0;
	for(const char* c = _dfastr;*c != '\0';++c)
	{
		uint32_t next_state = dfa_nextStateIdx(&dfa_parser[curr_state], *c);

		if(curr_state == 1 && next_state == 2)
		{
			++ (*_numStates);
			transitions_counter = 0;
		}
		if(curr_state == 5 && next_state == 5)
			++ transitions_counter;
		if(curr_state == 6 && next_state == 5)
			++ transitions_counter;

		if(next_state == 1)
		{
			if(transitions_counter > *_maxTransitions)
				*_maxTransitions = transitions_counter;
			transitions_counter = 0;
		}

		if(next_state == 0)
		{
			*_maxTransitions = 0;
			*_numStates = 0;
			return DFA_ERR_SYNTAX;
		}
		curr_state = next_state;
	}

	return DFA_SUCCESS;
}

uint32_t dfa_numStates(const DFADecider* _decider)
{
	if(_decider == NULL) return 0;

	return _decider->numStates;
}

uint32_t dfa_numTransitions(const DFAState* _state)
{
	if(_state == NULL) return 0;

	return _state->numTransitions;
}

uint32_t dfa_nextStateIdx(DFAState* _currState, char _nextChar)
{
	if(_currState == NULL) return DFA_NOSTATE;
	if(_currState->transitions == NULL)
		return _currState->defaultNext;

	for(uint32_t i=0;i<_currState->numTransitions;++i)
		if(_currState->transitions[i].nextChar == _nextChar)
			return _currState->transitions[i].toState;

	return _currState->defaultNext;
}

uint32_t dfa_stateIndex(IdStatePair* _container, uint32_t _containerSize, char _label)
{
	uint32_t idx = dfa_hashChar(_label) % _containerSize;
	while(_container[idx].state != _label)
	{
		if(_container[idx].state == '\0') return DFA_NOSTATE;
		idx = (idx + 1) % _containerSize;
	}

	return _container[idx].id;
}

DFAState* dfa_getState(const DFADecider* _decider, char _label)
{
	uint32_t state_index = dfa_stateIndex(_decider->lookup, _decider->stateLookupBucketSize, _label);

	return &_decider->dfa[state_index];
}

uint32_t dfa_hashChar(char _c)
{
	return (uint32_t)_c * 7 + 11;
}

DFAState* dfa_nextState(const DFADecider* _decider, DFAState* _currState, char _nextChar)
{
	if(_decider == NULL) return NULL;

	uint32_t idx = dfa_nextStateIdx(_currState, _nextChar);
	if(idx == DFA_NOSTATE) return NULL;
	return &_decider->dfa[idx];
}

void dfa_print(const DFADecider* _decider)
{
	printf("number of states: %u\n", _decider->numStates);
	for(uint32_t i=0;i<_decider->numStates;++i)
	{
		printf("state: %u, transition count: %u\n", i, _decider->dfa[i].numTransitions);
		printf("\tdefault next state: %u\n", _decider->dfa[i].defaultNext);
		for(uint32_t j=0;j<_decider->dfa[i].numTransitions;++j)
			printf("\tchar: %c to: %u\n", _decider->dfa[i].transitions[j].nextChar, _decider->dfa[i].transitions[j].toState);
	}
}

void dfa_freeDecider(DFADecider* _decider)
{
	dfa_freeState(_decider->dfa, _decider->numStates);
	free(_decider->lookup);

	free(_decider);
}

void dfa_freeState(DFAState* _state, uint32_t _count)
{
	for(uint32_t i=0;i<_count;++i)
		free(_state[i].transitions);

	free(_state);
}

void dfa_terminate()
{
	if(dfa_parser == NULL) return;

	dfa_freeState(dfa_parser, dfa_parserStateCount);
}
