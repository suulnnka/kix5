Test for model

CNN
RNN
ResNet
Transformer
Mamba

use MCTS

model head include
Value Head 
1 winning rate
1 predict final score
8*8 final ownership probability

Policy Head
8*8 predict next move probability
8*8 next move predict final score

model input
8*8 board my
8*8 board opponent
8*8 board move list
1 current trun number
1 last move winning rate
1 last move score