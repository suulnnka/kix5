#include "search.h"
#include "bit.h"
#include "flip.h"
#include "eval.h"

#include <stdio.h>

#define MAX_INT32 2147483647
#define FIXED_BIT 11

int32 kill_move[32][64];

int32 search_boolean(uint8 depth,uint64 my,uint64 opp){
	if ( depth == 0 ) {
		return ( bit_count(my) - bit_count(opp) ) << FIXED_BIT;
	}

	uint64 move_var = get_move(my, opp);

	if ( move_var == 0ull ) { //no move
		if ( get_move(opp,my) == 0ull ) {
			return ( bit_count(my) - bit_count(opp) ) << FIXED_BIT;
		} else {
			return -search_boolean(depth, opp, my);
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

	if( depth > 100 ) { // TODO need fix
		int32 move_value[64];
		for(int32 i = 0 ; i < move_count ; i ++){
			uint64 move = 1ull << move_list[i];
			uint64 flipped = flip_slow( my, opp, move );
			int32 score = -eval( opp ^ flipped, my | flipped | move );
			move_value[ move_list[i] ] = score;
		}
		for(int32 i = 1 ; i < move_count ; i ++){
			for(int32 j = i ; j > 0 ; j --){
				if( move_value[ move_list[j] ] > move_value[ move_list[j-1] ] ){
					int32 temp = move_list[j];
					move_list[j] = move_list[j-1];
					move_list[j-1] = temp;
				}else{
					break;
				}
			}
		}
	} else if ( depth > 4 ) {
		// kill sort
		for(int32 i = 1 ; i < move_count ; i ++){
			for(int32 j = i ; j > 0 ; j --){
				if( kill_move[depth][move_list[j]] > kill_move[depth][move_list[j-1]] ){
					int32 temp = move_list[j];
					move_list[j] = move_list[j-1];
					move_list[j-1] = temp;
				}else{
					break;
				}
			}
		}
	}

	int32 best_score = -MAX_INT32;
	for(uint16 i = 0 ; i < move_count ; i ++){
		uint64 move = 1ull << move_list[i];
		uint64 flipped = flip_slow( my, opp, move );
		int32 score = -search_boolean( depth - 1, opp ^ flipped, my | flipped | move );
		if(score > 0){
			kill_move[depth][move_list[i]] += 1;
			return score;
		}
		if(score > best_score){
			// kill_move[depth][move_list[i]] += 1;
			best_score = score;
		}
	}

	return best_score;
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

	if( depth > 100 ) { // TODO need fix
		int32 move_value[64];
		for(int32 i = 0 ; i < move_count ; i ++){
			uint64 move = 1ull << move_list[i];
			uint64 flipped = flip_slow( my, opp, move );
			int32 score = -eval( opp ^ flipped, my | flipped | move );
			move_value[ move_list[i] ] = score;
		}
		for(int32 i = 1 ; i < move_count ; i ++){
			for(int32 j = i ; j > 0 ; j --){
				if( move_value[ move_list[j] ] > move_value[ move_list[j-1] ] ){
					int32 temp = move_list[j];
					move_list[j] = move_list[j-1];
					move_list[j-1] = temp;
				}else{
					break;
				}
			}
		}
	} else if ( depth > 2 ) {
		// kill sort
		for(int32 i = 1 ; i < move_count ; i ++){
			for(int32 j = i ; j > 0 ; j --){
				if( kill_move[depth][move_list[j]] > kill_move[depth][move_list[j-1]] ){
					int32 temp = move_list[j];
					move_list[j] = move_list[j-1];
					move_list[j-1] = temp;
				}else{
					break;
				}
			}
		}
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
			//kill_move[depth][move_list[i]] += 1;
			a = t;
		}
		if(a >= beta){
			kill_move[depth][move_list[i]] += 1;
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

	if( depth > 100 ) { // TODO need fix
		int32 move_value[64];
		for(int32 i = 0 ; i < move_count ; i ++){
			uint64 move = 1ull << move_list[i];
			uint64 flipped = flip_slow( my, opp, move );
			int32 score = -eval( opp ^ flipped, my | flipped | move );
			move_value[ move_list[i] ] = score;
		}
		for(int32 i = 1 ; i < move_count ; i ++){
			for(int32 j = i ; j > 0 ; j --){
				if( move_value[ move_list[j] ] > move_value[ move_list[j-1] ] ){
					int32 temp = move_list[j];
					move_list[j] = move_list[j-1];
					move_list[j-1] = temp;
				}else{
					break;
				}
			}
		}
	} else if ( depth > 2 ) {
		// kill sort
		for(int32 i = 1 ; i < move_count ; i ++){
			for(int32 j = i ; j > 0 ; j --){
				if( kill_move[depth][move_list[j]] > kill_move[depth][move_list[j-1]] ){
					int32 temp = move_list[j];
					move_list[j] = move_list[j-1];
					move_list[j-1] = temp;
				}else{
					break;
				}
			}
		}
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
			//kill_move[depth][move_list[i]] += 1;
			a = t;
		}
		if(a >= beta){
			kill_move[depth][move_list[i]] += 1;
			return a;
		}
		b = a + 1;
	}
	return a;
}

uint16 search(uint8 depth,uint64 my,uint64 opp,int32 *value){

	// memset kill_move 0
	for(int32 i = 0 ; i < 32 ; i ++){
		for(int32 j = 0 ; j < 64 ; j ++){
			kill_move[i][j] = 0;
		}
	}

	uint64 move_var = get_move(my, opp);

	uint16 move_count = 0;
	uint16 move_list[60];

	while ( move_var != 0ull ) {
		uint64 move = move_var & ( -move_var );
		move_list[ move_count ] = bit_to_x( move );
		move_count ++;
		move_var = move_var ^ move;
	}

	// sort

	// 4 depth mid search for sort

	int32 best_score = -MAX_INT32;
	uint16 best_move = 0;

	for(uint16 i = 0 ; i < move_count ; i ++){
		uint64 move = 1ull << move_list[i];
		uint64 flipped = flip_slow( my, opp, move );

		// int32 score = -search_boolean( depth - 1, opp ^ flipped, my | flipped | move );
		int32 score = -search_exact( depth - 1, opp ^ flipped, my | flipped | move ,-MAX_INT32,MAX_INT32);

		if(score > best_score){
			best_score = score;
			best_move = move_list[i];
			(*value) = best_score;
		}

		if(score > 0){
			// return best_move;
		}
	}

	return best_move;
}
