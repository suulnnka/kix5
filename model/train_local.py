import torch
import torch.nn as nn
from torch.utils.data import Dataset, DataLoader
import numpy as np
import time
import argparse


MODEL_CLASS = 'cnn'
NUM_EPOCHS_TIMES = 3
BATCH_SIZE = 1024
USE_CUDA = True

# Egaroucid
# 1 level MSE 28.4050
# 2 level MSE 21.9735
# 3 level MSE 18.8479
# 4 level MSE 15.8776
# 5 level MSE 14.2098
# 6 level MSE 12.3423

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

        print(f"第 {epoch + 1} 轮训练完成")

    torch.save(model.state_dict(), MODEL_CLASS+'1.pth')
    print(f"模型已保存到 {MODEL_CLASS}1.pth")


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
