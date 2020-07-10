#ifndef SEARCH_H
#define SEARCH_H

#include "type.h"

#define MAX_INT32 2147483647
#define FIXED_BIT 10

void set_depth(int16 mid, int16 end);

uint16 search_end(uint8 depth,uint64 my,uint64 opp,int32 *value);

uint16 search(uint64 my,uint64 opp,int32 *value);

int32 search_evaluate(uint8 depth,uint64 my,uint64 opp,int32 alpha,int32 beta);

#endif
