Test for model

CNN
RNN
ResNet
SENet
DenseNet
Transformer
Mamba

katago Global Pooling,Bottleneck

跟参数量关系大,模型结构关系不大

use MCTS

model head include
Value Head 
1 winning rate
#1 predict final score

Policy Head
8*8 predict next move probability
#8*8 next move predict final score

model input
8*8 board my
8*8 board opponent
8*8 board move list / mobility
1 current trun number

传统估值到胜率的训练映射

胜率公式
P = 1 / (1 + e^(-ax))
a取大概0.2-0.3之间

选择公式
softmax(x_i) = exp(x_i / T) / sum(exp(x_j / T))
T取1左右
