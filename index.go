package main

import (
  "fmt"
  "time"
)

// ------------- random -------------
var random_current_seed int64 = 0

func random_range(max_number int32) int32 {
  random_current_seed = (random_current_seed * 214013 + 2531011) % 4294967296
  return int32(random_current_seed % int64(max_number))
}

func set_seed(seed int64) {
  random_current_seed = seed
}

// ------------- bit -------------

func bit_count(x uint64) uint8{
  var MASK1_uint8 uint64 = 0x5555555555555555
  var MASK2_uint8 uint64 = 0x3333333333333333
  var MASK4_uint8 uint64 = 0x0F0F0F0F0F0F0F0F
  x = x - ( ( x >> 1 ) & MASK1_uint8 )
  x = ( x & MASK2_uint8 ) + ( ( x >> 2 ) & MASK2_uint8 ) 
  x = (x+(x>>4)) & MASK4_uint8
  x = x * 0x0101010101010101
  return uint8(x>>56)
}

func bit_to_x(x uint64) uint8 {
  //使用欧拉回路算法生成的数组以及0x07EF3AE369961512
  euler_table := [64]uint8{
    63, 0,47, 1,56,48,27, 2,
    60,57,49,41,37,28,16, 3,
    61,54,58,35,52,50,42,21,
    44,38,32,29,23,17,11, 4,
    62,46,55,26,59,40,36,15,
    53,34,51,20,43,31,22,10,
    45,25,39,14,33,19,30, 9,
    24,13,18, 8,12, 7, 6, 5 }
  return euler_table[ ( x * 0x07EF3AE369961512 ) >> 58 ]
}

func x_to_bit(x int) uint64 {
  return uint64(1) << x
}

func white_line_mirror(x uint64) uint64 {
  var t uint64
  t = (x ^ (x >> 7)) & 0x00AA00AA00AA00AA
  x = x ^ t ^ (t << 7)
  t = (x ^ (x >> 14)) & 0x0000CCCC0000CCCC
  x = x ^ t ^ (t << 14)
  t = (x ^ (x >> 28)) & 0x00000000F0F0F0F0
  x = x ^ t ^ (t << 28)
  return x
}

func black_line_mirror(x uint64)uint64{
  var t uint64
  t = (x ^ (x >> 9)) & 0x0055005500550055
  x = x ^ t ^ (t << 9)
  t = (x ^ (x >> 18)) & 0x0000333300003333
  x = x ^ t ^ (t << 18)
  t = (x ^ (x >> 36)) & 0x000000000F0F0F0F
  x = x ^ t ^ (t << 36)
  return x
}

func vertical_mirror(x uint64)uint64{
  x = ((x >> 8) & 0x00FF00FF00FF00FF) | ((x << 8) & 0xFF00FF00FF00FF00)
  x = ((x >> 16) & 0x0000FFFF0000FFFF) | ((x << 16) & 0xFFFF0000FFFF0000)
  return ((x >> 32) & 0x00000000FFFFFFFF) | ((x << 32) & 0xFFFFFFFF00000000)
}

func horizontal_mirror(x uint64)uint64{
  x = ((x >> 1) & 0x5555555555555555) | ((x << 1) & 0xAAAAAAAAAAAAAAAA)
  x = ((x >> 2) & 0x3333333333333333) | ((x << 2) & 0xCCCCCCCCCCCCCCCC)
  return ((x >> 4) & 0x0F0F0F0F0F0F0F0F) | ((x << 4) & 0xF0F0F0F0F0F0F0F0)
}

func rotate_90(x uint64)uint64{
  return vertical_mirror(white_line_mirror(x))
}

func rotate_270(x uint64)uint64{
  return vertical_mirror(black_line_mirror(x))
}

func rotate_180(x uint64)uint64{
  x = ((x & 0x5555555555555555) << 1) | ((x & 0xAAAAAAAAAAAAAAAA) >> 1)
  x = ((x & 0x3333333333333333) << 2) | ((x & 0xCCCCCCCCCCCCCCCC) >> 2)
  x = ((x & 0x0F0F0F0F0F0F0F0F) << 4) | ((x & 0xF0F0F0F0F0F0F0F0) >> 4)
  x = ((x & 0x00FF00FF00FF00FF) << 8) | ((x & 0xFF00FF00FF00FF00) >> 8)
  x = ((x & 0x0000FFFF0000FFFF) << 16) | ((x & 0xFFFF0000FFFF0000) >> 16)
  return ((x & 0x00000000FFFFFFFF) << 32) | ((x & 0xFFFFFFFF00000000) >> 32)
}

// ------------- bitboard -------------

var WHITE int8 = 1
var BLACK int8 = -1

type Board struct{
  my uint64
  opp uint64
  fliped_list [60]uint64
  move_list [60][30]uint64
  move_count [60]int8
  color int8
  step int8
}

func new_board() *Board {
  board := new(Board)
  board.my = 0x0000000810000000
  board.opp = 0x0000001008000000
  board.color = BLACK
  board.step = 0
  return board
}

func (board *Board) pass() {
  tmp := board.my
  board.my = board.opp
  board.opp = tmp
  board.color = -board.color
}

func (board *Board) change_color() {
  tmp := board.my
  board.my = board.opp
  board.opp = tmp
  board.color = -board.color
  board.step++
}

func (board *Board) get_moves_array(moves uint64) {
  board.move_count[board.step] = 0
  for moves != 0 {
    move := moves & -moves
    board.move_list[board.step][board.move_count[board.step]] = move
    board.move_count[board.step]++
    moves = moves & (moves - 1)
  }
}

func (board *Board) draw() {
  my_char := "X"
  opp_char := "O"
  if board.color == WHITE {
    my_char = "O"
    opp_char = "X"
  }

  fmt.Println(" abcdefgh")
  for i:=0;i<8;i++ {
    fmt.Print(i+1)
    for j:=0;j<8;j++ {
      if (board.my & x_to_bit(i*8+j)) != 0 {
        fmt.Print(my_char)
      }else if (board.opp & x_to_bit(i*8+j)) != 0 {
        fmt.Print(opp_char)
      }else{
        fmt.Print(".")
      }
    }
    fmt.Println()
  }
}

func (board *Board) get_random_move() uint64 {
  rand := random_range(int32(board.move_count[board.step]))
  return board.move_list[board.step][rand]
}

func (board *Board) flip_slow(move uint64) uint64 {
  return flip_dir(board.my, board.opp&0x007E7E7E7E7E7E00, move, 9) |
	  flip_dir(board.my, board.opp&0x007E7E7E7E7E7E00, move, 7) |
	  flip_dir(board.my, board.opp&0x00FFFFFFFFFFFF00, move, 8) |
	  flip_dir(board.my, board.opp&0x7E7E7E7E7E7E7E7E, move, 1)
}

func flip_dir(my, opp, m uint64, dir uint8) uint64 {
  l := (m << dir) & opp
  l |= (l << dir) & opp
  l |= (l << dir) & opp
  l |= (l << dir) & opp
  l |= (l << dir) & opp
  l |= (l << dir) & opp

  r := (m >> dir) & opp
  r |= (r >> dir) & opp
  r |= (r >> dir) & opp
  r |= (r >> dir) & opp
  r |= (r >> dir) & opp
  r |= (r >> dir) & opp

  if ((l << dir) & my) == 0 { l = 0 }
  if ((r >> dir) & my) == 0 { r = 0 }

  return r | l
}

func get_some_move(my uint64,mask uint64,dir uint8) uint64 {
	flip := (( my << dir ) | ( my >> dir )) & mask
	flip |= (( flip << dir ) | (flip >> dir )) & mask
	flip |= (( flip << dir ) | (flip >> dir )) & mask
	flip |= (( flip << dir ) | (flip >> dir )) & mask
	flip |= (( flip << dir ) | (flip >> dir )) & mask
	flip |= (( flip << dir ) | (flip >> dir )) & mask
	return ( flip << dir ) | ( flip >> dir )
}

func (board *Board) get_moves() uint64{
	return (
    get_some_move(board.my , board.opp & 0x007E7E7E7E7E7E00 , 9 ) | 
    get_some_move(board.my , board.opp & 0x007E7E7E7E7E7E00 , 7 ) |
    get_some_move(board.my , board.opp & 0x7E7E7E7E7E7E7E7E , 1 ) |
    get_some_move(board.my , board.opp & 0x00FFFFFFFFFFFF00 , 8 ) ) &
    ( ^( board.my | board.opp) )
}

func pattern_vertical(my uint64,opp uint64) uint32{
  return uint32(my & 0x000000000000FF00 | opp & 0x00000000000000FF)
}

func pattern_horizontal(my uint64,opp uint64)uint32{
  my  = (my  & 0x0101010101010101) * 0x0102040810204080 >> 48 & 0x000000000000FF00
  opp = (opp & 0x0101010101010101) * 0x0102040810204080 >> 56 & 0x00000000000000FF
  return uint32(my | opp)
}

func pattern_diagonal(my uint64,opp uint64)uint32{
  my  = my  * 0x0101010101010101 >> 48 & 0x000000000000FF00
  opp = opp * 0x0101010101010101 >> 56 & 0x00000000000000FF
  return uint32(my | opp)
}

func (board *Board) eval() float32{

  var my  uint64 = board.my
  var opp uint64 = board.opp

  var pattern uint32

  pattern = pattern_vertical(my <<  8,opp >>  0)
  pattern = pattern_vertical(my >>  0,opp >>  8)
  pattern = pattern_vertical(my >>  8,opp >> 16)
  pattern = pattern_vertical(my >> 16,opp >> 24)
  pattern = pattern_vertical(my >> 24,opp >> 32)
  pattern = pattern_vertical(my >> 32,opp >> 40)
  pattern = pattern_vertical(my >> 40,opp >> 48)
  pattern = pattern_vertical(my >> 48,opp >> 56)

  pattern = pattern_diagonal(my&0x8040201008040201,opp&0x8040201008040201) // A1 -> H8 8bit
  
  pattern = pattern_diagonal(my&0x0080402010080402,opp&0x0080402010080402) >> 1 // A2 -> G8 7bit
  pattern = pattern_diagonal(my&0x0000804020100804,opp&0x0000804020100804) >> 2 // A3 -> F8 6bit
  pattern = pattern_diagonal(my&0x0000008040201008,opp&0x0000008040201008) >> 3 // A4 -> E8 5bit
  pattern = pattern_diagonal(my&0x0000000080402010,opp&0x0000000080402010) >> 4 // A5 -> D8 4bit

  pattern = pattern_diagonal(my&0x4020100804020100,opp&0x4020100804020100) // B1 -> H7 7bit
  pattern = pattern_diagonal(my&0x2010080402010000,opp&0x2010080402010000) // C1 -> H6 6bit
  pattern = pattern_diagonal(my&0x1008040201000000,opp&0x1008040201000000) // D1 -> H5 5bit
  pattern = pattern_diagonal(my&0x0804020100000000,opp&0x0804020100000000) // E1 -> H4 4bit

  // A8,A7,B8,A6,B7,C8
  pattern = uint32(
    ( my & x_to_bit( 7) ) >>  2 | // A8  7 -> 5 =  2
    ( my & x_to_bit(15) ) >> 11 | // A7 15 -> 4 = 11
    ( my & x_to_bit( 6) ) >>  3 | // B8  6 -> 3 =  3
    ( my & x_to_bit(23) ) >> 21 | // A6 23 -> 2 = 21
    ( my & x_to_bit(14) ) >> 13 | // B7 14 -> 1 = 13
    ( my & x_to_bit( 5) ) >>  5 ) // C8  5 -> 0 =  5

  // H1,G1,H2,F1,G2,H3
  pattern = uint32(
    ( my & x_to_bit(56) ) >> 51 | // H1 56 -> 5 = 51
    ( my & x_to_bit(57) ) >> 53 | // G1 57 -> 4 = 53
    ( my & x_to_bit(48) ) >> 45 | // H2 48 -> 3 = 45
    ( my & x_to_bit(58) ) >> 56 | // F1 58 -> 2 = 56
    ( my & x_to_bit(49) ) >> 48 | // G2 49 -> 1 = 48
    ( my & x_to_bit(40) ) >> 40 ) // H3 40 -> 0 = 40

  pattern = pattern_diagonal(my&0x0102040810204080,opp&0x0102040810204080) // A8 -> H1 8bit

  pattern = pattern_diagonal(my&0x0204081020408000,opp&0x0204081020408000) >> 1 // A7 -> G1 7bit
  pattern = pattern_diagonal(my&0x0408102040800000,opp&0x0408102040800000) >> 2 // A6 -> F1 6bit
  pattern = pattern_diagonal(my&0x0810204080000000,opp&0x0810204080000000) >> 3 // A5 -> E1 5bit
  pattern = pattern_diagonal(my&0x1020408000000000,opp&0x1020408000000000) >> 4 // A4 -> D1 4bit

  pattern = pattern_diagonal(my&0x0001020408102040,opp&0x0001020408102040) // B8 -> H2 7bit
  pattern = pattern_diagonal(my&0x0000010204081020,opp&0x0000010204081020) // C8 -> H3 6bit
  pattern = pattern_diagonal(my&0x0000000102040810,opp&0x0000000102040810) // D8 -> H4 5bit
  pattern = pattern_diagonal(my&0x0000000001020408,opp&0x0000000001020408) // E8 -> H5 4bit

  // A1,B1,A2,C1,B2,A3
  pattern = uint32(
    ( my & x_to_bit(63) ) >> 58 | // A1 63 -> 5 = 58
    ( my & x_to_bit(62) ) >> 58 | // B1 62 -> 4 = 58
    ( my & x_to_bit(55) ) >> 52 | // A2 55 -> 3 = 52
    ( my & x_to_bit(61) ) >> 59 | // C1 61 -> 2 = 59
    ( my & x_to_bit(54) ) >> 53 | // B2 54 -> 1 = 53
    ( my & x_to_bit(47) ) >> 47 ) // A3 47 -> 0 = 47
  
  // H8,G8,H7,F8,G7,H6
  pattern = uint32(
    ( my & x_to_bit( 0) ) <<  5 | // H8  0 -> 5 = -5
    ( my & x_to_bit( 1) ) <<  3 | // G8  1 -> 4 = -3
    ( my & x_to_bit( 8) ) >>  5 | // H7  8 -> 3 =  5
    ( my & x_to_bit( 2) ) >>  0 | // F8  2 -> 2 =  0
    ( my & x_to_bit( 9) ) >>  8 | // G7  9 -> 1 =  8
    ( my & x_to_bit(16) ) >> 16 ) // H6 16 -> 0 = 16

  my  = white_line_mirror(my)
  opp = white_line_mirror(opp)

  pattern = pattern_vertical(my <<  8,opp >>  0)
  pattern = pattern_vertical(my >>  0,opp >>  8)
  pattern = pattern_vertical(my >>  8,opp >> 16)
  pattern = pattern_vertical(my >> 16,opp >> 24)
  pattern = pattern_vertical(my >> 24,opp >> 32)
  pattern = pattern_vertical(my >> 32,opp >> 40)
  pattern = pattern_vertical(my >> 40,opp >> 48)
  pattern = pattern_vertical(my >> 48,opp >> 56)

  return float32(pattern)
}

func main() {
  set_seed(time.Now().Unix())
  board := new_board()
  //board.draw()
  for {
    moves := board.get_moves()
    if moves == 0 {
      board.pass()
      moves = board.get_moves()
      if moves == 0 {
        break
      }
    }
    board.get_moves_array(moves)
    move := board.get_random_move()
    fliped := board.flip_slow(move)

    board.my = board.my | fliped | move
    board.opp = board.opp ^ fliped
    board.change_color()

    //board.draw()
  }
  fmt.Println("Game Over")
  fmt.Println("my:",bit_count(board.my))
  fmt.Println("opp:",bit_count(board.opp))
}
