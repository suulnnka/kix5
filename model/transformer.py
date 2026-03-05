import torch
import torch.nn as nn
import torch.nn.functional as F
from base import BaseValueNet


CHANNELS = 128
NUM_HEADS = 8
NUM_LAYERS = 4
MLP_DIM = 256

# 550k

class ValueNet(BaseValueNet):
    def __init__(self):
        super().__init__()

        self.input_conv = nn.Conv2d(2, CHANNELS, kernel_size=3, padding=1, bias=False)
        self.input_bn = nn.BatchNorm2d(CHANNELS)

        self.cls_token = nn.Parameter(torch.zeros(1, 1, CHANNELS))
        self.pos_embed = nn.Parameter(torch.zeros(1, 65, CHANNELS))
        nn.init.trunc_normal_(self.pos_embed, std=0.02)

        encoder_layer = nn.TransformerEncoderLayer(
            d_model=CHANNELS,
            nhead=NUM_HEADS,
            dim_feedforward=MLP_DIM,
            dropout=0.1,
            activation='gelu',
            batch_first=True,
            norm_first=True
        )
        self.transformer = nn.TransformerEncoder(encoder_layer, num_layers=NUM_LAYERS)

        self.value_head = nn.Sequential(
            nn.LayerNorm(CHANNELS),
            nn.Linear(CHANNELS, 64),
            nn.GELU(),
            nn.Dropout(0.1),
            nn.Linear(64, 1)
        )

    def forward(self, x):
        B = x.size(0)

        x = F.relu(self.input_bn(self.input_conv(x)))

        x = x.flatten(2).permute(0, 2, 1)

        cls_tokens = self.cls_token.expand(B, -1, -1)
        x = torch.cat([cls_tokens, x], dim=1)

        x = x + self.pos_embed

        x = self.transformer(x)

        cls_output = x[:, 0]

        value = self.value_head(cls_output)

        return value
    
    @staticmethod
    def get_input_shape():
        return (2, 8, 8)
    
    @staticmethod
    def get_output_dim():
        return 1
