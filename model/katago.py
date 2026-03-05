import torch.nn as nn
import torch.nn.functional as F
from base import BaseValueNet

# 全局变量
CHANNELS = 192
BLOCKS = 12

class KataBottleneck(nn.Module):
    def __init__(self, in_channels, mid_channels):
        super().__init__()
        # 1. 压缩 (1x1)
        self.conv1 = nn.Conv2d(in_channels, mid_channels, kernel_size=1, bias=False)
        self.bn1 = nn.BatchNorm2d(mid_channels)
        # 2. 空间特征提取 (3x3)
        self.conv2 = nn.Conv2d(mid_channels, mid_channels, kernel_size=3, padding=1, bias=False)
        self.bn2 = nn.BatchNorm2d(mid_channels)
        # 3. 扩张 (1x1)
        self.conv3 = nn.Conv2d(mid_channels, in_channels, kernel_size=1, bias=False)
        self.bn3 = nn.BatchNorm2d(in_channels)
        
        # SENet 机制：基于全局池化调整通道响应
        self.se_fc = nn.Sequential(
            nn.AdaptiveAvgPool2d(1),
            nn.Conv2d(in_channels, in_channels // 4, kernel_size=1),
            nn.ReLU(inplace=True),
            nn.Conv2d(in_channels // 4, in_channels, kernel_size=1),
            nn.Sigmoid()
        )

    def forward(self, x):
        identity = x
        out = F.relu(self.bn1(self.conv1(x)))
        out = F.relu(self.bn2(self.conv2(out)))
        out = self.bn3(self.conv3(out))
        
        # 应用注意力机制
        out = out * self.se_fc(out)
        
        return F.relu(out + identity)

class ValueNet(BaseValueNet):
    def __init__(self):
        super().__init__()
        # 输入：[B, 2, 8, 8] -> 0层: 自己的棋子, 1层: 对方的棋子
        # (黑白棋中，通过轮换棋盘颜色，可以不需要第3个状态层)
        
        self.input_layer = nn.Sequential(
            nn.Conv2d(2, CHANNELS, kernel_size=3, padding=1, bias=False),
            nn.BatchNorm2d(CHANNELS),
            nn.ReLU(inplace=True)
        )
        
        # 主干网络 (Trunk)
        self.trunk = nn.Sequential(
            *[KataBottleneck(CHANNELS, mid_channels=CHANNELS // 2) for _ in range(BLOCKS)]
        )
        
        # 价值头 (Value Head) - 强化版
        # 结合了卷积特征和全局平均特征
        self.global_pool = nn.AdaptiveAvgPool2d(1)
        self.value_fc = nn.Sequential(
            nn.Linear(CHANNELS, 128),
            nn.ReLU(inplace=True),
            nn.Dropout(0.2), # 估值函数容易过拟合，加入 Dropout
            nn.Linear(128, 1)
        )

    def forward(self, x):
        x = self.input_layer(x)
        x = self.trunk(x)
        
        # 提取全局特征并展平
        x = self.global_pool(x).view(x.size(0), -1)
        value = self.value_fc(x)
        
        return value
    
    @staticmethod
    def get_input_shape():
        return (2, 8, 8)
    
    @staticmethod
    def get_output_dim():
        return 1

