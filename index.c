#include <stdio.h>
#include <stdlib.h>

typedef char				i8;
typedef unsigned char		u8;
typedef short int			i16;
typedef unsigned short int	u16;
typedef long				i32;
typedef unsigned long		u32;
typedef long long			i64;
typedef unsigned long long	u64;
typedef float               f32;
typedef double              f64;

#define POWER3_0 1
#define POWER3_1 3
#define POWER3_2 9
#define POWER3_3 27
#define POWER3_4 81
#define POWER3_5 243
#define POWER3_6 729
#define POWER3_7 2187
#define POWER3_8 6561

#define BOOK_PARA_NUM_4 45
#define BOOK_PARA_NUM_5 135
#define BOOK_PARA_NUM_6 378
#define BOOK_PARA_NUM_7 1134
#define BOOK_PARA_NUM_8 3321
#define BOOK_PARA_NUM_LEFT 405

#define BOOK_IN_BOOKS_CNT 15

u64 power3[9]={1,3,9,27,81,243,729,2187,6561};

u16 to_dict_bin_4[1<<8];
u16 to_dict_bin_5[1<<10];
u16 to_dict_bin_6[1<<12];
u16 to_dict_bin_7[1<<14];
u16 to_dict_bin_8[1<<16];
u16 to_dict_bin_left[1<<12];

struct BookForLearn{
    f32 line1[BOOK_PARA_NUM_8];
    f32 line2[BOOK_PARA_NUM_8];
    f32 line3[BOOK_PARA_NUM_8];
    f32 line4[BOOK_PARA_NUM_8];
    f32 diag8[BOOK_PARA_NUM_8];
    f32 diag7[BOOK_PARA_NUM_7];
    f32 diag6[BOOK_PARA_NUM_6];
    f32 diag5[BOOK_PARA_NUM_5];
    f32 diag4[BOOK_PARA_NUM_4];
    f32 diagl[BOOK_PARA_NUM_LEFT];
    u64 line1_cnt[BOOK_PARA_NUM_8];
    u64 line2_cnt[BOOK_PARA_NUM_8];
    u64 line3_cnt[BOOK_PARA_NUM_8];
    u64 line4_cnt[BOOK_PARA_NUM_8];
    u64 diag8_cnt[BOOK_PARA_NUM_8];
    u64 diag7_cnt[BOOK_PARA_NUM_7];
    u64 diag6_cnt[BOOK_PARA_NUM_6];
    u64 diag5_cnt[BOOK_PARA_NUM_5];
    u64 diag4_cnt[BOOK_PARA_NUM_4];
    u64 diagl_cnt[BOOK_PARA_NUM_LEFT];
};

struct BookForLearn books[15];

f32 random_f32(){
    return (rand()%2000 - 1000)/1000.0;
}

void init_book_for_learn(struct BookForLearn *book){
    srand(1);
    for(i16 i=0;i<(sizeof book->line1)/4;i++){book->line1[i]=random_f32();book->line1_cnt[i]=0;}
    for(i16 i=0;i<(sizeof book->line2)/4;i++){book->line2[i]=random_f32();book->line2_cnt[i]=0;}
    for(i16 i=0;i<(sizeof book->line3)/4;i++){book->line3[i]=random_f32();book->line3_cnt[i]=0;}
    for(i16 i=0;i<(sizeof book->line4)/4;i++){book->line4[i]=random_f32();book->line4_cnt[i]=0;}
    for(i16 i=0;i<(sizeof book->diag8)/4;i++){book->diag8[i]=random_f32();book->diag8_cnt[i]=0;}
    for(i16 i=0;i<(sizeof book->diag7)/4;i++){book->diag7[i]=random_f32();book->diag7_cnt[i]=0;}
    for(i16 i=0;i<(sizeof book->diag6)/4;i++){book->diag6[i]=random_f32();book->diag6_cnt[i]=0;}
    for(i16 i=0;i<(sizeof book->diag5)/4;i++){book->diag5[i]=random_f32();book->diag5_cnt[i]=0;}
    for(i16 i=0;i<(sizeof book->diag4)/4;i++){book->diag4[i]=random_f32();book->diag4_cnt[i]=0;}
    for(i16 i=0;i<(sizeof book->diagl)/4;i++){book->diagl[i]=random_f32();book->diagl_cnt[i]=0;}
}

void init_books_for_learn(){
    for(i16 i=0;i<BOOK_IN_BOOKS_CNT;i++){
        init_book_for_learn(&books[i]);
    }
}

void save_book_for_learn(FILE *fout,struct BookForLearn *book){
    fwrite(book->line1,4,(sizeof book->line1)/4,fout);
    fwrite(book->line2,4,(sizeof book->line2)/4,fout);
    fwrite(book->line3,4,(sizeof book->line3)/4,fout);
    fwrite(book->line4,4,(sizeof book->line4)/4,fout);
    fwrite(book->diag8,4,(sizeof book->diag8)/4,fout);
    fwrite(book->diag7,4,(sizeof book->diag7)/4,fout);
    fwrite(book->diag6,4,(sizeof book->diag6)/4,fout);
    fwrite(book->diag5,4,(sizeof book->diag5)/4,fout);
    fwrite(book->diag4,4,(sizeof book->diag4)/4,fout);
    fwrite(book->diagl,4,(sizeof book->diagl)/4,fout);

    fwrite(book->line1_cnt,8,(sizeof book->line1_cnt)/8,fout);
    fwrite(book->line2_cnt,8,(sizeof book->line2_cnt)/8,fout);
    fwrite(book->line3_cnt,8,(sizeof book->line3_cnt)/8,fout);
    fwrite(book->line4_cnt,8,(sizeof book->line4_cnt)/8,fout);
    fwrite(book->diag8_cnt,8,(sizeof book->diag8_cnt)/8,fout);
    fwrite(book->diag7_cnt,8,(sizeof book->diag7_cnt)/8,fout);
    fwrite(book->diag6_cnt,8,(sizeof book->diag6_cnt)/8,fout);
    fwrite(book->diag5_cnt,8,(sizeof book->diag5_cnt)/8,fout);
    fwrite(book->diag4_cnt,8,(sizeof book->diag4_cnt)/8,fout);
    fwrite(book->diagl_cnt,8,(sizeof book->diagl_cnt)/8,fout);
}

void save_books_for_learn(){
    FILE *fout=fopen("learn.bin","wb");
    for(i16 i=0;i<BOOK_IN_BOOKS_CNT;i++){
        save_book_for_learn(fout,&books[i]);
    }
    fclose(fout);
}

void read_book_for_learn(FILE *fin,struct BookForLearn *book){
    fread(book->line1,4,BOOK_PARA_NUM_8,fin);
    fread(book->line2,4,BOOK_PARA_NUM_8,fin);
    fread(book->line3,4,BOOK_PARA_NUM_8,fin);
    fread(book->line4,4,BOOK_PARA_NUM_8,fin);
    fread(book->diag8,4,BOOK_PARA_NUM_8,fin);
    fread(book->diag7,4,BOOK_PARA_NUM_7,fin);
    fread(book->diag6,4,BOOK_PARA_NUM_6,fin);
    fread(book->diag5,4,BOOK_PARA_NUM_5,fin);
    fread(book->diag4,4,BOOK_PARA_NUM_4,fin);
    fread(book->diagl,4,BOOK_PARA_NUM_LEFT,fin);
    
    fread(book->line1_cnt,8,BOOK_PARA_NUM_8,fin);
    fread(book->line2_cnt,8,BOOK_PARA_NUM_8,fin);
    fread(book->line3_cnt,8,BOOK_PARA_NUM_8,fin);
    fread(book->line4_cnt,8,BOOK_PARA_NUM_8,fin);
    fread(book->diag8_cnt,8,BOOK_PARA_NUM_8,fin);
    fread(book->diag7_cnt,8,BOOK_PARA_NUM_7,fin);
    fread(book->diag6_cnt,8,BOOK_PARA_NUM_6,fin);
    fread(book->diag5_cnt,8,BOOK_PARA_NUM_5,fin);
    fread(book->diag4_cnt,8,BOOK_PARA_NUM_4,fin);
    fread(book->diagl_cnt,8,BOOK_PARA_NUM_LEFT,fin);
}

void read_books_for_learn(){
    FILE *fin=fopen("learn.bin","rb");
    for(i16 i=0;i<BOOK_IN_BOOKS_CNT;i++){
        read_book_for_learn(fin,&books[i]);
    }
    fclose(fin);
}

// 18702 para 1 book
// 4 step 1 book
// 15 books
// total 280530 para
// 1 para u16 = 2 byte
// total 561060 byte

void init_dict(u16 *dict_bin,u8 length){
    u16 board[8] = {0,0,0,0,0,0,0,0};
    u16 pos = 0;
    u16 curr_pos = 0;
    i16 dict[power3[length]];
    for(u16 i=0;i<power3[length];i++){
        dict[i] = -1;
    }
    while(board[0]<3){
        u16 another = 0;
        for(u16 i=0;i<length;i++){
            another += board[i] * power3[i];
        }
        if(dict[another]!=-1){
            dict[pos] = dict[another];
        }else{
            dict[pos] = curr_pos;
            curr_pos ++;
        }

        u16 my=0;
        u16 opp=0;
        for(u16 i=0;i<length;i++){
            my = my << 1;
            opp = opp << 1;
            if(board[i]==1) my++;
            if(board[i]==2) opp++;
        }
        dict_bin[(my<<length)+opp] = dict[pos];

        board[length-1] += 1;
        for(u16 i=length-1;i>0;i--){
            if(board[i]==3){
                board[i-1]+=1;
                board[i]=0;
            }
        }
        pos++;
    }
    //printf("%d %d\n",length,curr_pos);
}

void init_dict_left(u16 *dict_bin,u8 length){
    u16 board[6] = {0,0,0,0,0,0};
    u16 pos = 0;
    u16 curr_pos = 0;
    i16 dict[power3[length]];
    for(u16 i=0;i<power3[length];i++){
        dict[i] = -1;
    }
    // high 3 2 1 low
    while(board[0]<3){
        u16 another = 0;
        another += board[5] * POWER3_0;
        another += board[3] * POWER3_1;
        another += board[4] * POWER3_2;
        another += board[0] * POWER3_3;
        another += board[1] * POWER3_4;
        another += board[2] * POWER3_5;

        if(dict[another]!=-1){
            dict[pos] = dict[another];
        }else{
            dict[pos] = curr_pos;
            curr_pos ++;
        }

        u16 my=0;
        u16 opp=0;
        for(u16 i=0;i<length;i++){
            my = my << 1;
            opp = opp << 1;
            if(board[i]==1) my++;
            if(board[i]==2) opp++;
        }
        dict_bin[(my<<length)+opp] = dict[pos];

        board[length-1] += 1;
        for(u16 i=length-1;i>0;i--){
            if(board[i]==3){
                board[i-1]+=1;
                board[i]=0;
            }
        }
        pos++;
    }
    //printf("%d %d\n",length,curr_pos);
}

void init_dict_bin(){
    init_dict(to_dict_bin_4,4);
    init_dict(to_dict_bin_5,5);
    init_dict(to_dict_bin_6,6);
    init_dict(to_dict_bin_7,7);
    init_dict(to_dict_bin_8,8);
    init_dict_left(to_dict_bin_left,6);
}

u16 pattern[38];
/* pattern
  1-8 row a-h
  9-16 col 1-8
  17 diag a1-h8
  18 diag a8-h1
  19-38 diag
    19-22 diag 7
      19 a2-g8
      20 b1-h7
      21 a7-g1
      22 b8-h2
    23-26 diag 6
      23 a3-f8
      24 c1-h6
      25 a6-f1
      26 c8-h3
    27-30 diag 5
      27 a4-e8
      28 d1-h5
      29 a5-e1
      30 d8-h4
    31-34 diag 4
      31 a5-d8
      32 e1-h4
      33 a4-d1
      34 e8-h5
  35 right up left
  36 left down left
  37 left up left
  38 right down left
*/

void board_to_pattern(u64 my,u64 opp){
    pattern[0] = ((my >> (64- 8) & 0xff) << 8) + (opp >> (64- 8) & 0xff);
    pattern[1] = ((my >> (64-16) & 0xff) << 8) + (opp >> (64-16) & 0xff);
    pattern[2] = ((my >> (64-24) & 0xff) << 8) + (opp >> (64-24) & 0xff);
    pattern[3] = ((my >> (64-32) & 0xff) << 8) + (opp >> (64-32) & 0xff);
    pattern[4] = ((my >> (64-40) & 0xff) << 8) + (opp >> (64-40) & 0xff);
    pattern[5] = ((my >> (64-48) & 0xff) << 8) + (opp >> (64-48) & 0xff);
    pattern[6] = ((my >> (64-56) & 0xff) << 8) + (opp >> (64-56) & 0xff);
    pattern[7] = ((my >> (64-64) & 0xff) << 8) + (opp >> (64-64) & 0xff);

    u64 mask1 = 0x0101010101010101ull;
    u64 mask2 = 0x0102040810204080ull;
    u64 mask3 = 0x8040201008040201ull;

    pattern[ 8] = ((my>>7&mask1)*mask2>>(64-8)<<8)+((opp>>7&mask1)*mask2>>(64-8));
    pattern[ 9] = ((my>>6&mask1)*mask2>>(64-8)<<8)+((opp>>6&mask1)*mask2>>(64-8));
    pattern[10] = ((my>>5&mask1)*mask2>>(64-8)<<8)+((opp>>5&mask1)*mask2>>(64-8));
    pattern[11] = ((my>>4&mask1)*mask2>>(64-8)<<8)+((opp>>4&mask1)*mask2>>(64-8));
    pattern[12] = ((my>>3&mask1)*mask2>>(64-8)<<8)+((opp>>3&mask1)*mask2>>(64-8));
    pattern[13] = ((my>>2&mask1)*mask2>>(64-8)<<8)+((opp>>2&mask1)*mask2>>(64-8));
    pattern[14] = ((my>>1&mask1)*mask2>>(64-8)<<8)+((opp>>1&mask1)*mask2>>(64-8));
    pattern[15] = ((my>>0&mask1)*mask2>>(64-8)<<8)+((opp>>0&mask1)*mask2>>(64-8));

    pattern[16] = ((my&mask3)*mask1>>(64-8)<<8)+((opp&mask3)*mask1>>(64-8));
    pattern[17] = ((my&mask2)*mask1>>(64-8)<<8)+((opp&mask2)*mask1>>(64-8));

    pattern[18] = ((my>> 8&mask3)*mask1>>(64-8)<<7)+((opp>> 8&mask3)*mask1>>(64-8));
    pattern[22] = ((my>>16&mask3)*mask1>>(64-8)<<6)+((opp>>16&mask3)*mask1>>(64-8));
    pattern[26] = ((my>>24&mask3)*mask1>>(64-8)<<5)+((opp>>24&mask3)*mask1>>(64-8));
    pattern[30] = ((my>>32&mask3)*mask1>>(64-8)<<4)+((opp>>32&mask3)*mask1>>(64-8));

    pattern[19] = ((my<< 8&mask3)*mask1>>(64-8)<<6)+((opp<< 8&mask3)*mask1>>(64-8)>>1);
    pattern[23] = ((my<<16&mask3)*mask1>>(64-8)<<4)+((opp<<16&mask3)*mask1>>(64-8)>>2);
    pattern[27] = ((my<<24&mask3)*mask1>>(64-8)<<2)+((opp<<24&mask3)*mask1>>(64-8)>>3);
    pattern[31] = ((my<<32&mask3)*mask1>>(64-8)<<0)+((opp<<32&mask3)*mask1>>(64-8)>>4);

    pattern[20] = ((my>> 8&mask2)*mask1>>(64-8)<<6)+((opp>> 8&mask2)*mask1>>(64-8)>>1);
    pattern[24] = ((my>>16&mask2)*mask1>>(64-8)<<4)+((opp>>16&mask2)*mask1>>(64-8)>>2);
    pattern[28] = ((my>>24&mask2)*mask1>>(64-8)<<2)+((opp>>24&mask2)*mask1>>(64-8)>>3);
    pattern[32] = ((my>>32&mask2)*mask1>>(64-8)<<0)+((opp>>32&mask2)*mask1>>(64-8)>>4);

    pattern[21] = ((my<< 8&mask2)*mask1>>(64-8)<<7)+((opp<< 8&mask2)*mask1>>(64-8));
    pattern[25] = ((my<<16&mask2)*mask1>>(64-8)<<6)+((opp<<16&mask2)*mask1>>(64-8));
    pattern[29] = ((my<<24&mask2)*mask1>>(64-8)<<5)+((opp<<24&mask2)*mask1>>(64-8));
    pattern[33] = ((my<<32&mask2)*mask1>>(64-8)<<4)+((opp<<32&mask2)*mask1>>(64-8));

    // right up left
    pattern[34] = ((my>>58&1)<<11)+((my>>49&1)<<10)+((my>>40&1)<<9)+
                ((my>>57&1)<<8)+((my>>48&1)<<7)+((my>>56&1)<<6)+
                ((opp>>58&1)<<5)+((opp>>49&1)<<4)+((opp>>40&1)<<3)+
                ((opp>>57&1)<<2)+((opp>>48&1)<<1)+((opp>>56&1)<<0);
    // left down left
    pattern[35] = ((my>>23&1)<<11)+((my>>14&1)<<10)+((my>>5&1)<<9)+
                ((my>>15&1)<<8)+((my>>6&1)<<7)+((my>>7&1)<<6)+
                ((opp>>23&1)<<5)+((opp>>14&1)<<4)+((opp>>5&1)<<3)+
                ((opp>>15&1)<<2)+((opp>>6&1)<<1)+((opp>>7&1)<<0);
    // left up left
    pattern[36] = ((my>>61&1)<<11)+((my>>54&1)<<10)+((my>>47&1)<<9)+
                ((my>>62&1)<<8)+((my>>55&1)<<7)+((my>>63&1)<<6)+
                ((opp>>61&1)<<5)+((opp>>54&1)<<4)+((opp>>47&1)<<3)+
                ((opp>>62&1)<<2)+((opp>>55&1)<<1)+((opp>>63&1)<<0);
    // right down left
    pattern[37] = ((my>>16&1)<<11)+((my>>9&1)<<10)+((my>>2&1)<<9)+
                ((my>>8&1)<<8)+((my>>1&1)<<7)+((my>>0&1)<<6)+
                ((opp>>16&1)<<5)+((opp>>9&1)<<4)+((opp>>2&1)<<3)+
                ((opp>>8&1)<<2)+((opp>>1&1)<<1)+((opp>>0&1)<<0);
}

f32 eval_board(u64 my,u64 opp,u16 space){
    board_to_pattern(my,opp);
    struct BookForLearn *book = &books[space/4];
    f32 score = 0;

    score += book->line1[to_dict_bin_8[pattern[0]]];
    score += book->line2[to_dict_bin_8[pattern[1]]];
    score += book->line3[to_dict_bin_8[pattern[2]]];
    score += book->line4[to_dict_bin_8[pattern[3]]];
    score += book->line4[to_dict_bin_8[pattern[4]]];
    score += book->line3[to_dict_bin_8[pattern[5]]];
    score += book->line2[to_dict_bin_8[pattern[6]]];
    score += book->line1[to_dict_bin_8[pattern[7]]];

    score += book->line1[to_dict_bin_8[pattern[8]]];
    score += book->line2[to_dict_bin_8[pattern[9]]];
    score += book->line3[to_dict_bin_8[pattern[10]]];
    score += book->line4[to_dict_bin_8[pattern[11]]];
    score += book->line4[to_dict_bin_8[pattern[12]]];
    score += book->line3[to_dict_bin_8[pattern[13]]];
    score += book->line2[to_dict_bin_8[pattern[14]]];
    score += book->line1[to_dict_bin_8[pattern[15]]];

    score += book->diag8[to_dict_bin_8[pattern[16]]];
    score += book->diag8[to_dict_bin_8[pattern[17]]];

    score += book->diag7[to_dict_bin_7[pattern[18]]];
    score += book->diag7[to_dict_bin_7[pattern[19]]];
    score += book->diag7[to_dict_bin_7[pattern[20]]];
    score += book->diag7[to_dict_bin_7[pattern[21]]];

    score += book->diag6[to_dict_bin_6[pattern[22]]];
    score += book->diag6[to_dict_bin_6[pattern[23]]];
    score += book->diag6[to_dict_bin_6[pattern[24]]];
    score += book->diag6[to_dict_bin_6[pattern[25]]];

    score += book->diag5[to_dict_bin_5[pattern[26]]];
    score += book->diag5[to_dict_bin_5[pattern[27]]];
    score += book->diag5[to_dict_bin_5[pattern[28]]];
    score += book->diag5[to_dict_bin_5[pattern[29]]];

    score += book->diag4[to_dict_bin_4[pattern[30]]];
    score += book->diag4[to_dict_bin_4[pattern[31]]];
    score += book->diag4[to_dict_bin_4[pattern[32]]];
    score += book->diag4[to_dict_bin_4[pattern[33]]];

    score += book->diagl[to_dict_bin_left[pattern[34]]];
    score += book->diagl[to_dict_bin_left[pattern[35]]];
    score += book->diagl[to_dict_bin_left[pattern[36]]];
    score += book->diagl[to_dict_bin_left[pattern[37]]];
    
    return score;
}

int main(){
    init_books_for_learn();
    save_books_for_learn();
    read_books_for_learn();
    u64 a = 0x0101010101010101ull;
    u64 b = 0x0102040810204080ull;
    u64 c = a*b >> (64-8) ;
    printf("%llu\n",c);

    init_dict_bin();
    return 0;
}

