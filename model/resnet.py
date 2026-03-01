import torch.nn as nn
import torch.nn.functional as F
from base import BaseValueNet

# 全局变量
CHANNELS = 96
BLOCKS_NUM = 6

# 1M 参数
# MSE Loss 30.3376

class ResNetBlock(nn.Module):
    def __init__(self, channels):
        super().__init__()
        self.conv1 = nn.Conv2d(channels, channels, kernel_size=3, padding=1, bias=False)
        self.bn1 = nn.BatchNorm2d(channels)
        self.conv2 = nn.Conv2d(channels, channels, kernel_size=3, padding=1, bias=False)
        self.bn2 = nn.BatchNorm2d(channels)
    
    def forward(self, x):
        residual = x

        out = self.conv1(x)
        out = self.bn1(out)
        out = F.relu(out)
        
        out = self.conv2(out)
        out = self.bn2(out)
        
        out += residual
        out = F.relu(out)
        
        return out


class ValueNet(BaseValueNet):
    def __init__(self):
        super().__init__()

        # 初始卷积层
        self.conv1 = nn.Conv2d(2, CHANNELS, 3, padding=1, bias=False)
        self.bn1 = nn.BatchNorm2d(CHANNELS)
        self.relu = nn.ReLU()
        
        # 创建ResNet block
        self.blocks = nn.ModuleList()
        for i in range(BLOCKS_NUM):
            self.blocks.append(ResNetBlock(CHANNELS))
        
        #self.pool = nn.AdaptiveAvgPool2d(1)
        #self.fc = nn.Linear(CHANNELS, 1)

        self.value_conv = nn.Conv2d(CHANNELS, 2, kernel_size=1)
        self.value_bn = nn.BatchNorm2d(2)
        self.fc_extra = nn.Linear(2*8*8, 64)
        self.fc_out = nn.Linear(64, 1)


    def forward(self, x):
        x = self.relu(self.bn1(self.conv1(x)))
        
        for block in self.blocks:
            x = block(x)
        
        #x = self.pool(x)
        #x = x.view(x.size(0), -1)
        #x = self.fc(x)

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
