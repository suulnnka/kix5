#include <stdio.h>
#include <string.h>
#include "search.h"
#include "flip.h"
#include "bit.h"
#include "type.h"
#include "eval.h"

unsigned long long black = 0ull;
unsigned long long white = 0ull;

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

int fforum(){
    FILE *in = fopen("fforum.txt","r");

    char str[1000];

    int n = 0;
    while( fgets(str,1000,in) != NULL ){
        n ++;
        if(n == 26){break;}
        int len = strlen(str);

        white = 0ull;
        black = 0ull;

        int space_number = 0;
        for(int x = 0 ; x < 8 ; x ++){
            for(int y = 0 ; y < 8 ; y ++){

                char pos = str[x * 8 + y];

                if( pos == 'X'){
                    set_board(x+1,y+1,-1);
                }
                if( pos == 'O' ){
                    set_board(x+1,y+1,1);
                }
                if( pos == '-' ){
                    space_number ++;
                }

            }
        }

        // printf("%d %c\n",n,str[65]);

        unsigned long long my,opp;

        if(str[65] == 'X'){
            my = black;
            opp = white;
        }

        if(str[65] == 'O'){
            my = white;
            opp = black;
        }

        int32 best_score;

        // int pos = end_search(space_number,my,opp,&best_score);
        int pos = search_end(space_number,my,opp,&best_score);

        float best_score_float = (float)best_score / ( 1 << 11) ;

        int x = pos / 8;
        int y = pos % 8;

        char out[3];
        out[0] = 'A' + y;
        out[1] = '1' + x;
        out[2] = '\0';

        printf("%d %s:%.0f\n",n,out,best_score_float);

    }

    return 0;
}

float run(int show){
    
    white = 0ull;
    black = 0ull;

    int16 WHITE = 1;
    int16 BLACK = -1;

    set_board(4,4,WHITE);
    set_board(5,5,WHITE);
    set_board(4,5,BLACK);
    set_board(5,4,BLACK);
    
    int16 color = BLACK;

    float total_diff = 0;

    while(1){

        uint64 my  = color == WHITE ? white : black ;
        uint64 opp = color == WHITE ? black : white ;

        uint64 move_var = get_move(my,opp);

        if ( move_var == 0ull ) { //no move
            if ( get_move(opp,my) == 0ull ) {
                break;
            } else {
                color = -color;
                continue;
            }
        }

        int32 value;

        uint16 pos = search(my,opp,&value);
        uint64 move = 1ull << pos;

        if(show){
            float best_score_float = (float)value / ( 1 << 11) ;

            int x = pos / 8;
            int y = pos % 8;

            char out[3];
            out[0] = 'A' + y;
            out[1] = '1' + x;
            out[2] = '\0';

            printf("%s:%.0f\n", out, best_score_float);
        }

        int32 diff = learn_position(my,opp,value);

        diff = diff > 0 ? diff : - diff ;
        total_diff += (float)diff / (1 << 11);

		uint64 flipped = flip_slow( my, opp, move );

        my =  my | flipped | move ;
        opp = opp ^ flipped ;

        white = color == WHITE ? my : opp ;
        black = color == WHITE ? opp : my ;

        color = - color;
    };

    if(show){
        if( bit_count(white) == bit_count(black) ) {
            printf("draw.\n");
        }
        if( bit_count(white) > bit_count(black) ) {
            printf("%d:%d white win.\n",bit_count(white),bit_count(black));
        }
        if( bit_count(white) < bit_count(black) ) {
            printf("%d:%d black win.\n",bit_count(white),bit_count(black));
        }
    }

    return total_diff;

}

int main(){

    // fforum();

    // return 0;

    load_eval();
    int A = 100;
    int B = 200;
    set_depth(4,8);
    for(int i = 0 ; i < A ; i ++){
        float total_diff = 0;
        for(int i = 0 ; i < B ; i ++){
            total_diff += run(0);
        }
        printf("diff: %.1f\n",total_diff / B);
    }
    save_eval();
    set_depth(8,16);
    run(1);
    return 0;
}