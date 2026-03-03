import torch
import torch.nn as nn
import torch.nn.functional as F
from base import BaseValueNet

# 全局变量
CHANNELS = 128
BLOCKS_NUM = 3
GROWTH_RATE = 48
COMPRESSION_RATE = 0.5


class DenseLayer(nn.Module):
    def __init__(self, in_channels):
        super().__init__()
        self.bn = nn.BatchNorm2d(in_channels)
        self.relu = nn.ReLU(inplace=True)
        self.conv = nn.Conv2d(in_channels, GROWTH_RATE, kernel_size=3, padding=1, bias=False)
    
    def forward(self, x):
        out = self.conv(self.relu(self.bn(x)))
        out = torch.cat([x, out], dim=1)
        return out


class DenseBlock(nn.Module):
    def __init__(self, in_channels, num_layers):
        super().__init__()
        self.layers = nn.ModuleList()
        for i in range(num_layers):
            self.layers.append(DenseLayer(in_channels + i * GROWTH_RATE))
    
    def forward(self, x):
        for layer in self.layers:
            x = layer(x)
        return x


class TransitionLayer(nn.Module):
    def __init__(self, in_channels):
        super().__init__()
        out_channels = int(in_channels * COMPRESSION_RATE)
        self.bn = nn.BatchNorm2d(in_channels)
        self.relu = nn.ReLU(inplace=True)
        self.conv = nn.Conv2d(in_channels, out_channels, kernel_size=1, bias=False)
    
    def forward(self, x):
        out = self.conv(self.relu(self.bn(x)))
        return out


class ValueNet(BaseValueNet):
    def __init__(self):
        super().__init__()

        # 初始卷积层
        self.conv1 = nn.Conv2d(2, CHANNELS, 3, padding=1, bias=False)
        self.bn1 = nn.BatchNorm2d(CHANNELS)
        self.relu = nn.ReLU()
        
        # 创建DenseBlock和TransitionLayer
        self.blocks = nn.ModuleList()
        self.transitions = nn.ModuleList()
        
        current_channels = CHANNELS
        for i in range(BLOCKS_NUM):
            # 添加DenseBlock
            self.blocks.append(DenseBlock(current_channels, 5))  # 每个DenseBlock包含5个DenseLayer
            current_channels += 5 * GROWTH_RATE
            
            # 添加TransitionLayer（除了最后一个block）
            if i < BLOCKS_NUM - 1:
                self.transitions.append(TransitionLayer(current_channels))
                current_channels = int(current_channels * COMPRESSION_RATE)
        
        # 最终的分类层
        self.value_conv = nn.Conv2d(current_channels, 2, kernel_size=1)
        self.value_bn = nn.BatchNorm2d(2)
        self.fc_extra = nn.Linear(2*8*8, 64)
        self.fc_out = nn.Linear(64, 1)


    def forward(self, x):
        x = self.relu(self.bn1(self.conv1(x)))
        
        for i, block in enumerate(self.blocks):
            x = block(x)
            if i < len(self.transitions):
                x = self.transitions[i](x)
        
        x = self.value_conv(x)
        x = self.value_bn(x)
        x = self.relu(x)

        x = x.view(x.size(0), -1)
        x = self.fc_extra(x)
        x = self.relu(x)
        x = self.fc_out(x)
        
        return x
    
    @staticmethod
    def get_input_shape():
        return (2, 8, 8)
    
    @staticmethod
    def get_output_dim():
        return 1
