#include "search.h"
#include "bit.h"
#include "flip.h"
#include "eval.h"

#include <stdio.h>

#define MAX_INT32 2147483647
#define FIXED_BIT 11

int32 depth_end = 12;
int32 depth_mid = 6;

void set_depth(int16 mid, int16 end){
	depth_mid = mid;
	depth_end = end;
}

void sort_insert(uint16 *list, uint16 count, int32 *weight){
	for(int32 i = 1 ; i < count ; i ++){
		for(int32 j = i ; j > 0 ; j --){
			if( weight[ list[j] ] > weight[ list[j-1] ] ){
				int32 temp = list[j];
				list[j] = list[j-1];
				list[j-1] = temp;
			}else{
				break;
			}
		}
	}
}

int32 search_exact(uint8 depth,uint64 my,uint64 opp,int32 alpha,int32 beta){
	if ( depth == 0 ) {
		return ( bit_count(my) - bit_count(opp) ) << FIXED_BIT;
	}

	uint64 move_var = get_move(my, opp);

	if ( move_var == 0ull ) { //no move
		if ( get_move(opp,my) == 0ull ) {
			return ( bit_count(my) - bit_count(opp) ) << FIXED_BIT;
		} else {
			return -search_exact(depth, opp, my,-beta,-alpha);
		}
	}

	uint16 move_count = 0;
	uint16 move_list[60];

	while ( move_var != 0ull ) {
		uint64 move = move_var & ( -move_var );
		move_list[ move_count ] = bit_to_x( move );
		move_count ++;
		move_var = move_var ^ move;
	}

	if(depth > 5){
		int32 move_value[64];
		for(int32 i = 0 ; i < move_count ; i ++){
			uint64 move = 1ull << move_list[i];
			uint64 flipped = flip_slow( my, opp, move );
			move_value[ move_list[i] ] = -bit_count(get_move( opp ^ flipped, my | flipped | move ));
		}

		sort_insert(move_list, move_count, move_value);
	}

	int32 a = alpha;
	int32 b = beta;
	for(uint16 i = 0 ; i < move_count ; i ++){
		uint64 move = 1ull << move_list[i];
		uint64 flipped = flip_slow( my, opp, move );
		int32 t = -search_exact(depth - 1, opp ^ flipped, my | flipped | move,-b, -a);
		if(t > a && t < beta && i > 0 && depth > 1){
      		a = -search_exact( depth - 1, opp ^ flipped, my | flipped | move, -beta, -t );
		}
		if(t > a){
			a = t;
		}
		if(a >= beta){
			return a;
		}
		b = a + 1;
	}
	return a;
}

int32 search_evaluate(uint8 depth,uint64 my,uint64 opp,int32 alpha,int32 beta){
	if ( depth == 0 ) {
		return eval(my,opp);
	}

	uint64 move_var = get_move(my, opp);

	if ( move_var == 0ull ) { //no move
		if ( get_move(opp,my) == 0ull ) {
			return ( bit_count(my) - bit_count(opp) ) << FIXED_BIT;
		} else {
			return -search_evaluate(depth, opp, my,-beta,-alpha);
		}
	}

	uint16 move_count = 0;
	uint16 move_list[60];

	while ( move_var != 0ull ) {
		uint64 move = move_var & ( -move_var );
		move_list[ move_count ] = bit_to_x( move );
		move_count ++;
		move_var = move_var ^ move;
	}

	if(depth > 4){
		int32 move_value[64];
		for(int32 i = 0 ; i < move_count ; i ++){
			uint64 move = 1ull << move_list[i];
			uint64 flipped = flip_slow( my, opp, move );
			move_value[ move_list[i] ] = -bit_count(get_move( opp ^ flipped, my | flipped | move ));
		}

		sort_insert(move_list, move_count, move_value);
	}

	int32 a = alpha;
	int32 b = beta;
	for(uint16 i = 0 ; i < move_count ; i ++){
		uint64 move = 1ull << move_list[i];
		uint64 flipped = flip_slow( my, opp, move );
		int32 t = -search_evaluate(depth - 1, opp ^ flipped, my | flipped | move,-b, -a);
		if(t > a && t < beta && i > 0 && depth > 1){
      		a = -search_evaluate( depth - 1, opp ^ flipped, my | flipped | move, -beta, -t );
		}
		if(t > a){
			a = t;
		}
		if(a >= beta){
			return a;
		}
		b = a + 1;
	}
	return a;
}

uint16 search_mid(uint8 depth, uint64 my, uint64 opp, int32 *value){
	uint64 move_var = get_move(my, opp);

	uint16 move_count = 0;
	uint16 move_list[60];

	while ( move_var != 0ull ) {
		uint64 move = move_var & ( -move_var );
		move_list[ move_count ] = bit_to_x( move );
		move_count ++;
		move_var = move_var ^ move;
	}

	int32 move_value[64];
	for(int32 i = 0 ; i < move_count ; i ++){
		uint64 move = 1ull << move_list[i];
		uint64 flipped = flip_slow( my, opp, move );
		move_value[ move_list[i] ] = -get_move( opp ^ flipped, my | flipped | move );
	}

	sort_insert(move_list, move_count, move_value);

	uint64 move = 1ull << move_list[0];
	uint64 flipped = flip_slow( my, opp, move );
	int32 best_score = -search_evaluate( depth - 1, opp ^ flipped, my | flipped | move , -MAX_INT32, MAX_INT32);
	uint16 best_move = move_list[0];
	(*value) = best_score;

	for(uint16 i = 1 ; i < move_count ; i ++){
		uint64 move = 1ull << move_list[i];
		uint64 flipped = flip_slow( my, opp, move );
		int32 score = -search_evaluate( depth - 1, opp ^ flipped, my | flipped | move , -best_score-1, -best_score);

		if(score > best_score){
			best_score = -search_evaluate( depth - 1, opp ^ flipped, my | flipped | move , -MAX_INT32, -score);
			best_move = move_list[i];
			(*value) = best_score;
		}
	}

	return best_move;
}

uint16 search_end(uint8 depth, uint64 my, uint64 opp, int32 *value){
	uint64 move_var = get_move(my, opp);

	uint16 move_count = 0;
	uint16 move_list[60];

	while ( move_var != 0ull ) {
		uint64 move = move_var & ( -move_var );
		move_list[ move_count ] = bit_to_x( move );
		move_count ++;
		move_var = move_var ^ move;
	}

	int32 move_value[64];
	for(int32 i = 0 ; i < move_count ; i ++){
		uint64 move = 1ull << move_list[i];
		uint64 flipped = flip_slow( my, opp, move );
		move_value[ move_list[i] ] = -get_move( opp ^ flipped, my | flipped | move );
	}

	sort_insert(move_list, move_count, move_value);

	uint64 move = 1ull << move_list[0];
	uint64 flipped = flip_slow( my, opp, move );
	int32 best_score = -search_exact( depth - 1, opp ^ flipped, my | flipped | move , -MAX_INT32, MAX_INT32);
	uint16 best_move = move_list[0];
	(*value) = best_score;

	for(uint16 i = 1 ; i < move_count ; i ++){
		uint64 move = 1ull << move_list[i];
		uint64 flipped = flip_slow( my, opp, move );
		int32 score = -search_exact( depth - 1, opp ^ flipped, my | flipped | move , -best_score-1, -best_score);

		if(score > best_score){
			best_score = -search_exact( depth - 1, opp ^ flipped, my | flipped | move , -MAX_INT32, -score);
			best_move = move_list[i];
			(*value) = best_score;
		}
	}

	return best_move;
}

uint16 search(uint64 my,uint64 opp,int32 *value){
	int16 space = 64 - bit_count(my) - bit_count(opp);

	if(space <= depth_end){
		return search_end(space,my,opp,value);
	}else{
		return search_mid(depth_mid,my,opp,value);
	}
}
