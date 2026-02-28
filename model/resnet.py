import torch.nn as nn
import torch.nn.functional as F
from base import BaseValueNet

# 1.77M 参数

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

        channels = 128

        # 初始卷积层
        self.conv1 = nn.Conv2d(2, channels, 3, padding=1, bias=False)
        self.bn1 = nn.BatchNorm2d(channels)
        self.relu = nn.ReLU()
        
        # 6个形状相同的ResNet block
        self.block1 = ResNetBlock(channels)
        self.block2 = ResNetBlock(channels)
        self.block3 = ResNetBlock(channels)
        self.block4 = ResNetBlock(channels)
        self.block5 = ResNetBlock(channels)
        self.block6 = ResNetBlock(channels)
        
        # 池化和全连接层
        self.pool = nn.AdaptiveAvgPool2d(1)
        self.fc = nn.Linear(channels, 1)
    
    def forward(self, x):
        x = self.relu(self.bn1(self.conv1(x)))
        
        x = self.block1(x)
        x = self.block2(x)
        x = self.block3(x)
        x = self.block4(x)
        x = self.block5(x)
        x = self.block6(x)
        
        x = self.pool(x)
        x = x.view(x.size(0), -1)
        x = self.fc(x)
        
        return x
    
    @staticmethod
    def get_input_shape():
        return (2, 8, 8)
    
    @staticmethod
    def get_output_dim():
        return 1
