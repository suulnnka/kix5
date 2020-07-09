#ifndef EVAL_H
#define EVAL_H

#include "type.h"

int32 eval(uint64 my,uint64 opp);

int32 learn_position(uint64 my,uint64 opp,int32 v_prime);
void load_eval();
void save_eval();

#endif
