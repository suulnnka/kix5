import torch
import torch.nn as nn
from torch.utils.data import Dataset, DataLoader
import numpy as np
import time
import argparse
import os


MODEL_CLASS = 'katago'
NUM_EPOCHS_TIMES = 13
BATCH_SIZE = 1024
USE_CUDA = True

# Egaroucid
# 1 level MSE 28.4050
# 2 level MSE 21.9735
# 3 level MSE 18.8479
# 4 level MSE 15.8776
# 5 level MSE 14.2098
# 6 level MSE 12.3423

# mlp  50k 参数 MSE Loss 51.1104
# cnn 537k 参数 MSE Loss 33.5506
# resnet channels 96 block 6 1M MSE Loss 31.1249 21
# resnet channels 64 block 6 MSE Loss 32.0040 26
# resnet channels 128 block 8 MSE Loss 30.2003 16
# densenet 1.9M 训练速度太慢了 116s 一轮 14轮 还有下降潜力 但是算了吧 28.9290 / 21.9259
# katago 1.7M 更慢 132s 一轮 20轮 27.8908 / 23.7038 没看出什么优势

def get_device():
    if USE_CUDA and torch.cuda.is_available():
        return torch.device('cuda')
    elif torch.backends.mps.is_available():
        return torch.device('mps')
    else:
        return torch.device('cpu')

device = get_device()
print(f"使用设备: {device}")


class OthelloDataset(Dataset):
    def __init__(self, filename):
        with np.load(filename) as data:
            self.boards = torch.tensor(data['boards'].reshape(-1, 2, 8, 8), dtype=torch.float32)
            self.values = torch.tensor(data['values'], dtype=torch.float32)
        print(f"数据量: {len(self.boards)}")

    def __len__(self):
        return len(self.boards)

    def __getitem__(self, idx):
        return self.boards[idx], self.values[idx]


def train(model_class=None):
    if model_class is None:
        model_class = __import__(MODEL_CLASS).ValueNet

    model = model_class()
    model = model.to(device)
    if os.path.exists(MODEL_CLASS+'1.pth'):
        model.load_state_dict(torch.load(MODEL_CLASS+'1.pth'))
        print("已加载已有模型")
    else:
        print("新建模型")

    criterion = nn.MSELoss()
    optimizer = torch.optim.Adam(model.parameters(), lr=0.001)

    npz_files = "dataset.npz"
    dataset = OthelloDataset(npz_files)

    for epoch in range(NUM_EPOCHS_TIMES):
        print(f"\n=== 第 {epoch + 1}/{NUM_EPOCHS_TIMES} 轮训练 ===")
        
        train_loader = DataLoader(dataset, batch_size=BATCH_SIZE, shuffle=True)

        model.train()
        total_train_loss = 0.0
        total_samples = 0

        epoch_start_time = time.time()
        for boards, values in train_loader:
            boards = boards.to(device)
            values = values.to(device)
            optimizer.zero_grad()
            outputs = model(boards)
            loss = criterion(outputs, values)
            loss.backward()
            optimizer.step()
            total_train_loss += loss.item() * boards.size(0)
            total_samples += boards.size(0)

        avg_train_loss = total_train_loss / total_samples
        epoch_time = time.time() - epoch_start_time
        print(f"轮次 {epoch + 1}, Loss: {avg_train_loss:.4f}, Time: {epoch_time:.2f}s")

        torch.save(model.state_dict(), MODEL_CLASS+'1.pth')
        print(f"模型已保存到 {MODEL_CLASS}1.pth")
        
        print(f"第 {epoch + 1} 轮训练完成，开始评估...")
        evaluate()

    print("所有轮次训练完成")


def evaluate(model_class=None):
    if model_class is None:
        model_class = __import__(MODEL_CLASS).ValueNet
    
    model = model_class()
    model = model.to(device)
    model.load_state_dict(torch.load(MODEL_CLASS+'1.pth'))
    model.eval()

    criterion = nn.MSELoss()

    total_loss = 0.0
    total_samples = 0
        
    dataset = OthelloDataset("check_data.npz")
    val_loader = DataLoader(dataset, batch_size=BATCH_SIZE, shuffle=False)
    
    with torch.no_grad():
        for boards, values in val_loader:
            boards = boards.to(device)
            values = values.to(device)
            outputs = model(boards)
            loss = criterion(outputs, values)
            total_loss += loss.item() * boards.size(0)
            total_samples += boards.size(0)
    
    avg_loss = total_loss / total_samples
    print(f"\n最终MSE: {avg_loss:.4f}")


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Othello Value Net Training')
    parser.add_argument('-t', '--train', action='store_true', help='训练模型')
    parser.add_argument('-e', '--evaluate', action='store_true', help='评估模型')
    args = parser.parse_args()

    if not args.train and not args.evaluate:
        args.train = True
        args.evaluate = True

    if args.train:
        train()
    if args.evaluate:
        evaluate()
