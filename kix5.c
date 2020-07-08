/*
unsigned long long black = 0ull;
unsigned long long white = 0ull;

float best_score;

void set_board(int x,int y,int c){
	unsigned long long i = 1ull;

	int pos = ( x - 1 ) * 8 + ( y - 1 );
	i = i << pos;

	if( c == 1 ){
		white |= i ;
	}
	if( c == -1 ){
		black |= i ;
	}
}

int get_best_move(int deep,int *bb,int c){
	int m;
	if(c == 1){
		//m = search(deep,white,black,&best_score);
	}else{
		//m = search(deep,black,white,&best_score);
	}

	int x = m/8 + 1;
	int y = m%8 + 1;

	return x*10+y;
}

int get_best_score(){
	return best_score;
}
*/