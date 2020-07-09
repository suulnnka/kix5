#include "flip.h"
#include "type.h"

uint64 flip_slow(uint64 my, uint64 opp, uint64 m){
	return (flip_dir(my,opp&0x007E7E7E7E7E7E00ull,m,9)
		| flip_dir(my,opp&0x007E7E7E7E7E7E00ull,m,7)
		| flip_dir(my,opp&0x00FFFFFFFFFFFF00ull,m,8)
		| flip_dir(my,opp&0x7E7E7E7E7E7E7E7Eull,m,1));
}
uint64 flip_dir(uint64 my,uint64 opp,uint64 m,uint8 dir){
	uint64 l,r;
	l  = ( m << dir ) & opp;	r  = ( m >> dir ) & opp;
	l |= ( l << dir ) & opp;	r |= ( r >> dir ) & opp;
	l |= ( l << dir ) & opp;	r |= ( r >> dir ) & opp;
	l |= ( l << dir ) & opp;	r |= ( r >> dir ) & opp;
	l |= ( l << dir ) & opp;	r |= ( r >> dir ) & opp;
	l |= ( l << dir ) & opp;	r |= ( r >> dir ) & opp;

	((( l << dir ) & my ) == 0ull )?l = 0ull:(void)0;
	((( r >> dir ) & my ) == 0ull )?r = 0ull:(void)0;

	return r|l;
}
