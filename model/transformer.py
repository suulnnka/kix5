import torch
import torch.nn as nn
import torch.nn.functional as F
import math
from base import BaseValueNet


CHANNELS = 128
NUM_HEADS = 8
NUM_LAYERS = 4
MLP_DIM = 256


class ValueNet(BaseValueNet):
    def __init__(self):
        super().__init__()

        self.input_conv = nn.Conv2d(2, CHANNELS, kernel_size=3, padding=1, bias=False)
        self.input_bn = nn.BatchNorm2d(CHANNELS)

        self.cls_token = nn.Parameter(torch.zeros(1, 1, CHANNELS))
        
        self.freqs_cis = self._precompute_freqs_cis()
        
        self.blocks = nn.ModuleList([
            TransformerBlock(CHANNELS, NUM_HEADS, MLP_DIM) 
            for _ in range(NUM_LAYERS)
        ])
        
        self.norm = nn.LayerNorm(CHANNELS)

        self.value_head = nn.Sequential(
            nn.Linear(CHANNELS, 64),
            nn.GELU(),
            nn.Dropout(0.1),
            nn.Linear(64, 1)
        )

    def _precompute_freqs_cis(self):
        half_dim = CHANNELS // 2
        freqs = 1.0 / (10000.0 ** (torch.arange(0, half_dim).float() / half_dim))
        t = torch.arange(65)
        freqs = torch.outer(t, freqs)
        freqs_cis = torch.polar(torch.ones_like(freqs), freqs)
        return freqs_cis
    
    def _apply_rope(self, x):
        B, N, H, D = x.shape
        half_D = D // 2
        
        x_real = x[..., :half_D]
        x_imag = x[..., half_D:]
        
        freqs_cis = self.freqs_cis[:N, :half_D].unsqueeze(0).unsqueeze(2)
        
        cos = freqs_cis.real
        sin = freqs_cis.imag
        
        x_out_real = x_real * cos - x_imag * sin
        x_out_imag = x_real * sin + x_imag * cos
        
        return torch.cat([x_out_real, x_out_imag], dim=-1)

    def forward(self, x):
        B = x.size(0)

        x = F.relu(self.input_bn(self.input_conv(x)))

        x = x.flatten(2).permute(0, 2, 1)

        cls_tokens = self.cls_token.expand(B, -1, -1)
        x = torch.cat([cls_tokens, x], dim=1)

        for block in self.blocks:
            x = block(x, self._apply_rope)

        x = self.norm(x)

        cls_output = x[:, 0]

        value = self.value_head(cls_output)

        return value
    
    @staticmethod
    def get_input_shape():
        return (2, 8, 8)
    
    @staticmethod
    def get_output_dim():
        return 1


class TransformerBlock(nn.Module):
    def __init__(self, dim: int, num_heads: int, mlp_dim: int):
        super().__init__()
        self.norm1 = nn.LayerNorm(dim)
        self.attn = RopeAttention(dim, num_heads)
        self.norm2 = nn.LayerNorm(dim)
        self.mlp = nn.Sequential(
            nn.Linear(dim, mlp_dim),
            nn.GELU(),
            nn.Dropout(0.1),
            nn.Linear(mlp_dim, dim),
            nn.Dropout(0.1)
        )
        
    def forward(self, x, apply_rope_fn):
        x = x + self.attn(self.norm1(x), apply_rope_fn)
        x = x + self.mlp(self.norm2(x))
        return x


class RopeAttention(nn.Module):
    def __init__(self, dim: int, num_heads: int):
        super().__init__()
        self.num_heads = num_heads
        self.head_dim = dim // num_heads
        self.scale = self.head_dim ** -0.5
        
        self.qkv = nn.Linear(dim, dim * 3)
        self.proj = nn.Linear(dim, dim)
        
    def forward(self, x, apply_rope_fn):
        B, N, C = x.shape
        
        qkv = self.qkv(x).reshape(B, N, 3, self.num_heads, self.head_dim)
        q, k, v = qkv.unbind(2)
        
        q = apply_rope_fn(q)
        k = apply_rope_fn(k)
        
        attn = (q @ k.transpose(-2, -1)) * self.scale
        attn = attn.softmax(dim=-1)
        
        x = (attn @ v).transpose(1, 2).reshape(B, N, C)
        x = self.proj(x)
        
        return x
