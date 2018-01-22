#ifndef ENTROPY_HPP
#define ENTROPY_HPP

#include <iostream>
#include <queue>
#include <map>
#include <cstdlib>
#include <cmath>
#include <inttypes.h>

typedef enum {
	NONE, 				//0
	PRINT,		 		//1
	DIRECT_FILE,			//2
	TO_QUEUE,			//3
	TO_LOCK_FREE_QUEUE,	//4
	TO_ZEROMQ,			//5
} type_t;

typedef struct {
    type_t type;
    uint64_t	 key_hv;
} op_t;

// extern std::map <int, double> counters;  
// extern std::list <bm_op_t> window;  
// extern int windowSize;
// extern double sum;
// extern double entropy;

double log_n(double t);
int initEntropy(int _windowSize);
void initializeCounterIfNecessary(op_t element);
void updateEntropyWithoutElement(op_t element);
void removeLastElement();
void updateWindowContent(op_t element);
void updateEntropy(op_t element);
void updateCounters(op_t element);
void processEntropy(op_t element);
double getEntropyValue();
#endif