#include "bit.h"

uint8 bit_count(uint64 x){
	return __builtin_popcountll(x);
}

uint8 bit_to_x(uint64 x){
	return __builtin_ctzll(x);
}
uint64 get_some_move(uint64 my,uint64 mask,uint8 dir){
	uint64 flip;
	flip = (( my << dir ) | ( my >> dir )) & mask;
	flip |= (( flip << dir ) | (flip >> dir )) & mask;
	flip |= (( flip << dir ) | (flip >> dir )) & mask;
	flip |= (( flip << dir ) | (flip >> dir )) & mask;
	flip |= (( flip << dir ) | (flip >> dir )) & mask;
	flip |= (( flip << dir ) | (flip >> dir )) & mask;
	return ( flip << dir ) | ( flip >> dir );
}
uint64 get_move(uint64 my,uint64 opp){
	return (get_some_move(my , opp & 0x007E7E7E7E7E7E00ull , 9 )
		| get_some_move(my , opp & 0x007E7E7E7E7E7E00ull , 7 )
		| get_some_move(my , opp & 0x7E7E7E7E7E7E7E7Eull , 1 )
		| get_some_move(my , opp & 0x00FFFFFFFFFFFF00ull , 8 ))
		& ( ~( my | opp) ) ;
}

uint8 reverse_bit(uint8 n){
	n = n >> 4 | n << 4;
	n = ( ( n & 0xcc ) >> 2 ) | ( ( n & 0x33 ) << 2 );
	n = ( ( n & 0xaa ) >> 1 ) | ( ( n & 0x55 ) << 1 );
	return n;
}
