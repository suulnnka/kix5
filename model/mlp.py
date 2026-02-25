import torch.nn as nn
from base import BaseValueNet

# 50k 参数
# MSE Loss 48.9492

class ValueNet(BaseValueNet):
    def __init__(self):
        super().__init__()
        self.fc1 = nn.Linear(2 * 8 * 8, 256)
        self.fc2 = nn.Linear(256, 64)
        self.fc3 = nn.Linear(64, 1)
        self.relu = nn.ReLU()

    def forward(self, x):
        x = x.view(x.size(0), -1)
        x = self.relu(self.fc1(x))
        x = self.relu(self.fc2(x))
        x = self.fc3(x)
        return x

    @staticmethod
    def get_input_shape():
        return (2, 8, 8)

    @staticmethod
    def get_output_dim():
        return 1
