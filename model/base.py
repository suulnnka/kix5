import torch.nn as nn


class BaseValueNet(nn.Module):
    def __init__(self):
        super().__init__()

    def forward(self, x):
        raise NotImplementedError("子类必须实现 forward 方法")

    @staticmethod
    def get_input_shape():
        raise NotImplementedError("子类必须实现 get_input_shape 方法")

    @staticmethod
    def get_output_dim():
        raise NotImplementedError("子类必须实现 get_output_dim 方法")
