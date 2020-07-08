#include <stdio.h>
#include <string.h>
#include "search.h"
#include "bit.h"

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

int main(){
    FILE *in = fopen("fforum.txt","r");

    char str[1000];

    int n = 0;
    while( fgets(str,1000,in) != NULL ){
        n ++;
        if(n == 22){break;}
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
        int pos = search(space_number,my,opp,&best_score);

        float best_score_float = (float)best_score / ( 1 << 11) ;

        int x = pos / 8;
        int y = pos % 8;

        char out[3];
        out[0] = 'A' + y;
        out[1] = '1' + x;
        out[2] = '\0';

        printf("%d %s:%.1f\n",n,out,best_score_float);

    }

    return 0;
}
