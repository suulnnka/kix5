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

// ------------- bitboard -------------
func bit_count(x uint64) uint8{
  var MASK1_uint8 uint64 = 0x5555555555555555
  var MASK2_uint8 uint64 = 0x3333333333333333
  var MASK4_uint8 uint64 = 0x0F0F0F0F0F0F0F0F
  x = x - ( ( x >> 1 ) & MASK1_uint8 );
  x = ( x & MASK2_uint8 ) + ( ( x >> 2 ) & MASK2_uint8 ) ;
  x = (x+(x>>4)) & MASK4_uint8;
  x = x * 0x0101010101010101;
  return uint8(x>>56);
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
  return euler_table[ ( x * 0x07EF3AE369961512 ) >> 58 ];
}

func x_to_bit(x uint8) uint64 {
  return uint64(1) << x
}

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
  for i:=uint8(0);i<8;i++ {
    fmt.Print(i+1)
    for j:=uint8(0);j<8;j++ {
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

func main() {
  set_seed(time.Now().Unix())
  board := new_board()
  board.draw()
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

    board.my = board.my | fliped | move;
    board.opp = board.opp ^ fliped;
    board.change_color()

    board.draw()
  }
  fmt.Println("Game Over")
  fmt.Println("my:",bit_count(board.my))
  fmt.Println("opp:",bit_count(board.opp))
}
