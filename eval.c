#include "type.h"
#include "bit.h"
#include "eval.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BOOK_DEEPTH 4
#define BOOK_NUMBER (60 / BOOK_DEEPTH)

// #define LEARNING_RATE 0.0065
#define LEARNING_RATE 0.00023

#define one_book_total_number (6561 * 5 + 2187 + 729 + 243 + 81 + 729)

int32 power_3_array[] = { 1, 3, 9, 27, 81, 243, 729, 2187, 6561, 19683, 59049 };

int16 eval_array[BOOK_NUMBER][one_book_total_number]; 

int32 straight_1_start = 0;
int32 straight_2_start = 6561 * 1;
int32 straight_3_start = 6561 * 2;
int32 straight_4_start = 6561 * 3;
int32 diagonall_8_start = 6561 * 4;
int32 diagonall_7_start = 6561 * 5;
int32 diagonall_6_start = 6561 * 5 + 2187;
int32 diagonall_5_start = 6561 * 5 + 2187 + 729;
int32 diagonall_4_start = 6561 * 5 + 2187 + 729 + 243;
int32 diagonall_321_start = 6561 * 5 + 2187 + 729 + 243 + 81;

int32 eval_pattern(int16 *array, int8 x1, int8 x2, int8 x3, int8 x4, int8 x5, int8 x6, int8 x7, int8 x8){
    int32 pattern = power_3_array[0] * x1 +
                    power_3_array[1] * x2 + 
                    power_3_array[2] * x3 + 
                    power_3_array[3] * x4 + 
                    power_3_array[4] * x5 + 
                    power_3_array[5] * x6 + 
                    power_3_array[6] * x7 + 
                    power_3_array[7] * x8 ;
    return array[pattern];
}

void update_pattern(int16 *array, int8 x1, int8 x2, int8 x3, int8 x4, int8 x5, int8 x6, int8 x7, int8 x8,int32 diff){
    int32 pattern = power_3_array[0] * x1 +
                    power_3_array[1] * x2 + 
                    power_3_array[2] * x3 + 
                    power_3_array[3] * x4 + 
                    power_3_array[4] * x5 + 
                    power_3_array[5] * x6 + 
                    power_3_array[6] * x7 + 
                    power_3_array[7] * x8 ;
    array[pattern] += diff;
}

int32 eval(uint64 my,uint64 opp){

    int32 my_count  = bit_count(my);
    int32 opp_count = bit_count(opp);

    int32 book_number = ( my_count + opp_count - 4 ) / BOOK_DEEPTH ;

    int8 b[64];

    for( int8 i = 0 ; i < 64 ; i ++){
        b[i] = ( my & 1ull ) + ( ( opp & 1ull ) << 1 ) ;
        my = my >> 1;
        opp = opp >> 1;
    }

    int32 value = 0;

    int16 *array = eval_array[book_number];

    value += eval_pattern(array + straight_1_start,b[A1],b[A2],b[A3],b[A4],b[A5],b[A6],b[A7],b[A8]);
    value += eval_pattern(array + straight_2_start,b[B1],b[B2],b[B3],b[B4],b[B5],b[B6],b[B7],b[B8]);
    value += eval_pattern(array + straight_3_start,b[C1],b[C2],b[C3],b[C4],b[C5],b[C6],b[C7],b[C8]);
    value += eval_pattern(array + straight_4_start,b[D1],b[D2],b[D3],b[D4],b[D5],b[D6],b[D7],b[D8]);
    value += eval_pattern(array + straight_4_start,b[E1],b[E2],b[E3],b[E4],b[E5],b[E6],b[E7],b[E8]);
    value += eval_pattern(array + straight_3_start,b[F1],b[F2],b[F3],b[F4],b[F5],b[F6],b[F7],b[F8]);
    value += eval_pattern(array + straight_2_start,b[G1],b[G2],b[G3],b[G4],b[G5],b[G6],b[G7],b[G8]);
    value += eval_pattern(array + straight_1_start,b[H1],b[H2],b[H3],b[H4],b[H5],b[H6],b[H7],b[H8]);

    value += eval_pattern(array + straight_1_start,b[A1],b[B1],b[C1],b[D1],b[E1],b[F1],b[G1],b[H1]);
    value += eval_pattern(array + straight_2_start,b[A2],b[B2],b[C2],b[D2],b[E2],b[F2],b[G2],b[H2]);
    value += eval_pattern(array + straight_3_start,b[A3],b[B3],b[C3],b[D3],b[E3],b[F3],b[G3],b[H3]);
    value += eval_pattern(array + straight_4_start,b[A4],b[B4],b[C4],b[D4],b[E4],b[F4],b[G4],b[H4]);
    value += eval_pattern(array + straight_4_start,b[A5],b[B5],b[C5],b[D5],b[E5],b[F5],b[G5],b[H5]);
    value += eval_pattern(array + straight_3_start,b[A6],b[B6],b[C6],b[D6],b[E6],b[F6],b[G6],b[H6]);
    value += eval_pattern(array + straight_2_start,b[A7],b[B7],b[C7],b[D7],b[E7],b[F7],b[G7],b[H7]);
    value += eval_pattern(array + straight_1_start,b[A8],b[B8],b[C8],b[D8],b[E8],b[F8],b[G8],b[H8]);

    value += eval_pattern(array + diagonall_8_start,b[A1],b[B2],b[C3],b[D4],b[E5],b[F6],b[G7],b[H8]);

    value += eval_pattern(array + diagonall_7_start,b[A2],b[B3],b[C4],b[D5],b[E6],b[F7],b[G8],0);
    value += eval_pattern(array + diagonall_6_start,b[A3],b[B4],b[C5],b[D6],b[E7],b[F8],0,0);
    value += eval_pattern(array + diagonall_5_start,b[A4],b[B5],b[C6],b[D7],b[E8],0,0,0);
    value += eval_pattern(array + diagonall_4_start,b[A5],b[B6],b[C7],b[D8],0,0,0,0);
    value += eval_pattern(array + diagonall_321_start,b[A6],b[B7],b[C8],b[A7],b[B8],b[A8],0,0);

    value += eval_pattern(array + diagonall_7_start,b[B1],b[C2],b[D3],b[E4],b[F5],b[G6],b[H7],0);
    value += eval_pattern(array + diagonall_6_start,b[C1],b[D2],b[E3],b[F4],b[G5],b[H6],0,0);
    value += eval_pattern(array + diagonall_5_start,b[D1],b[E2],b[F3],b[G4],b[H5],0,0,0);
    value += eval_pattern(array + diagonall_4_start,b[E1],b[F2],b[G3],b[H4],0,0,0,0);
    value += eval_pattern(array + diagonall_321_start,b[F1],b[G2],b[H3],b[G1],b[H2],b[H1],0,0);
    
    value += eval_pattern(array + diagonall_8_start,b[A8],b[B7],b[C6],b[D5],b[E4],b[F3],b[G2],b[H1]);

    value += eval_pattern(array + diagonall_7_start,b[A7],b[B6],b[C5],b[D4],b[E3],b[F2],b[G1],0);
    value += eval_pattern(array + diagonall_6_start,b[A6],b[B5],b[C4],b[D3],b[E2],b[F1],0,0);
    value += eval_pattern(array + diagonall_5_start,b[A5],b[B4],b[C3],b[D2],b[E1],0,0,0);
    value += eval_pattern(array + diagonall_4_start,b[A4],b[B3],b[C2],b[D1],0,0,0,0);
    value += eval_pattern(array + diagonall_321_start,b[A3],b[B2],b[C1],b[A2],b[B1],b[A1],0,0);

    value += eval_pattern(array + diagonall_7_start,b[B8],b[C7],b[D6],b[E5],b[F4],b[G3],b[H2],0);
    value += eval_pattern(array + diagonall_6_start,b[C8],b[D7],b[E6],b[F5],b[G4],b[H3],0,0);
    value += eval_pattern(array + diagonall_5_start,b[D8],b[E7],b[F6],b[G5],b[H4],0,0,0);
    value += eval_pattern(array + diagonall_4_start,b[E8],b[F7],b[G6],b[H5],0,0,0,0);
    value += eval_pattern(array + diagonall_321_start,b[F8],b[G7],b[H6],b[G8],b[H7],b[H8],0,0);

    return value;
}

int32 learn_position(uint64 my,uint64 opp,int32 v_prime){
    int32 v = eval(my,opp);

    float diff_prime = ( v_prime - v ) * LEARNING_RATE;
    int32 diff = (int32)diff_prime;

    int count = bit_count(my) + bit_count(opp);
    int book_number = ( count - 4 ) / BOOK_DEEPTH ;

    int8 b[64];

    for( int8 i = 0 ; i < 64 ; i ++){
        b[i] = ( my & 1ull ) + ( ( opp & 1ull ) << 1 ) ;
        my = my >> 1;
        opp = opp >> 1;
    }

    int16 *array = eval_array[book_number];
    
    update_pattern(array + straight_1_start,b[A1],b[A2],b[A3],b[A4],b[A5],b[A6],b[A7],b[A8],diff);
    update_pattern(array + straight_2_start,b[B1],b[B2],b[B3],b[B4],b[B5],b[B6],b[B7],b[B8],diff);
    update_pattern(array + straight_3_start,b[C1],b[C2],b[C3],b[C4],b[C5],b[C6],b[C7],b[C8],diff);
    update_pattern(array + straight_4_start,b[D1],b[D2],b[D3],b[D4],b[D5],b[D6],b[D7],b[D8],diff);
    update_pattern(array + straight_4_start,b[E1],b[E2],b[E3],b[E4],b[E5],b[E6],b[E7],b[E8],diff);
    update_pattern(array + straight_3_start,b[F1],b[F2],b[F3],b[F4],b[F5],b[F6],b[F7],b[F8],diff);
    update_pattern(array + straight_2_start,b[G1],b[G2],b[G3],b[G4],b[G5],b[G6],b[G7],b[G8],diff);
    update_pattern(array + straight_1_start,b[H1],b[H2],b[H3],b[H4],b[H5],b[H6],b[H7],b[H8],diff);

    update_pattern(array + straight_1_start,b[A1],b[B1],b[C1],b[D1],b[E1],b[F1],b[G1],b[H1],diff);
    update_pattern(array + straight_2_start,b[A2],b[B2],b[C2],b[D2],b[E2],b[F2],b[G2],b[H2],diff);
    update_pattern(array + straight_3_start,b[A3],b[B3],b[C3],b[D3],b[E3],b[F3],b[G3],b[H3],diff);
    update_pattern(array + straight_4_start,b[A4],b[B4],b[C4],b[D4],b[E4],b[F4],b[G4],b[H4],diff);
    update_pattern(array + straight_4_start,b[A5],b[B5],b[C5],b[D5],b[E5],b[F5],b[G5],b[H5],diff);
    update_pattern(array + straight_3_start,b[A6],b[B6],b[C6],b[D6],b[E6],b[F6],b[G6],b[H6],diff);
    update_pattern(array + straight_2_start,b[A7],b[B7],b[C7],b[D7],b[E7],b[F7],b[G7],b[H7],diff);
    update_pattern(array + straight_1_start,b[A8],b[B8],b[C8],b[D8],b[E8],b[F8],b[G8],b[H8],diff);

    update_pattern(array + diagonall_8_start,b[A1],b[B2],b[C3],b[D4],b[E5],b[F6],b[G7],b[H8],diff);

    update_pattern(array + diagonall_7_start,b[A2],b[B3],b[C4],b[D5],b[E6],b[F7],b[G8],0,diff);
    update_pattern(array + diagonall_6_start,b[A3],b[B4],b[C5],b[D6],b[E7],b[F8],0,0,diff);
    update_pattern(array + diagonall_5_start,b[A4],b[B5],b[C6],b[D7],b[E8],0,0,0,diff);
    update_pattern(array + diagonall_4_start,b[A5],b[B6],b[C7],b[D8],0,0,0,0,diff);
    update_pattern(array + diagonall_321_start,b[A6],b[B7],b[C8],b[A7],b[B8],b[A8],0,0,diff);

    update_pattern(array + diagonall_7_start,b[B1],b[C2],b[D3],b[E4],b[F5],b[G6],b[H7],0,diff);
    update_pattern(array + diagonall_6_start,b[C1],b[D2],b[E3],b[F4],b[G5],b[H6],0,0,diff);
    update_pattern(array + diagonall_5_start,b[D1],b[E2],b[F3],b[G4],b[H5],0,0,0,diff);
    update_pattern(array + diagonall_4_start,b[E1],b[F2],b[G3],b[H4],0,0,0,0,diff);
    update_pattern(array + diagonall_321_start,b[F1],b[G2],b[H3],b[G1],b[H2],b[H1],0,0,diff);
    
    update_pattern(array + diagonall_8_start,b[A8],b[B7],b[C6],b[D5],b[E4],b[F3],b[G2],b[H1],diff);

    update_pattern(array + diagonall_7_start,b[A7],b[B6],b[C5],b[D4],b[E3],b[F2],b[G1],0,diff);
    update_pattern(array + diagonall_6_start,b[A6],b[B5],b[C4],b[D3],b[E2],b[F1],0,0,diff);
    update_pattern(array + diagonall_5_start,b[A5],b[B4],b[C3],b[D2],b[E1],0,0,0,diff);
    update_pattern(array + diagonall_4_start,b[A4],b[B3],b[C2],b[D1],0,0,0,0,diff);
    update_pattern(array + diagonall_321_start,b[A3],b[B2],b[C1],b[A2],b[B1],b[A1],0,0,diff);

    update_pattern(array + diagonall_7_start,b[B8],b[C7],b[D6],b[E5],b[F4],b[G3],b[H2],0,diff);
    update_pattern(array + diagonall_6_start,b[C8],b[D7],b[E6],b[F5],b[G4],b[H3],0,0,diff);
    update_pattern(array + diagonall_5_start,b[D8],b[E7],b[F6],b[G5],b[H4],0,0,0,diff);
    update_pattern(array + diagonall_4_start,b[E8],b[F7],b[G6],b[H5],0,0,0,0,diff);
    update_pattern(array + diagonall_321_start,b[F8],b[G7],b[H6],b[G8],b[H7],b[H8],0,0,diff);

    return v_prime - v ;

}

void init_eval(char *array){
    int16 *data = (int16*)array;

    for(int32 book_number = 0 ; book_number < BOOK_NUMBER ; book_number ++){
        for(int32 i = 0 ; i < one_book_total_number ; i ++){
            eval_array[book_number][i] = data[ book_number * one_book_total_number + i ];
        }
    }
}

void load_eval(){
    for(int32 book_number = 0 ; book_number < BOOK_NUMBER ; book_number ++){
        for(int32 i = 0 ; i < one_book_total_number ; i ++){
            eval_array[book_number][i] = 0;
        }
    }
    FILE *fin=fopen("model.bin","rb");
    if(fin) {
        char *data;

        fseek(fin,0,SEEK_END);
        long length = ftell(fin);
        rewind(fin);
        data = (char *)malloc(length + 1);

        memset(data, 0, length + 1);
        fread(data, 1, length, fin);

        init_eval(data);

        free(data);
        fclose(fin);
    }
}

void save_eval(){
    int16 data[ BOOK_NUMBER * one_book_total_number ];

    for(int32 book_number = 0 ; book_number < BOOK_NUMBER ; book_number ++){
        for(int32 i = 0 ; i < one_book_total_number ; i ++){
            data[ book_number * one_book_total_number + i ] = eval_array[book_number][i];
        }
    }

    FILE *fout = fopen("model.bin", "w");
    fwrite(data, BOOK_NUMBER * one_book_total_number * 2 , 1, fout );
    fclose(fout);
}
