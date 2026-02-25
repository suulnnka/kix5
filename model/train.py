import torch
import torch.nn as nn
from torch.utils.data import Dataset, DataLoader
import os
import glob
import numpy as np
import time
import argparse


NUM_EPOCHS_TIMES = 5
BATCH_SIZE = 8192 * 4
USE_CUDA = True


def get_device():
    if USE_CUDA and torch.cuda.is_available():
        return torch.device('cuda')
    elif torch.backends.mps.is_available():
        return torch.device('mps')
    else:
        return torch.device('cpu')

device = get_device()
print(f"使用设备: {device}")


def preprocess_data():
    data_dir = r"..\Egaroucid_Train_Data\0001_egaroucid_7_5_1_lv17"
    txt_files = sorted(glob.glob(os.path.join(data_dir, "*.txt")))
    print(f"找到 {len(txt_files)} 个txt文件")

    batch_size = 1000000

    for file_idx, txt_file in enumerate(txt_files):
        batch_file = os.path.join(data_dir, f"train_batch_{file_idx}.npz")
        if os.path.exists(batch_file):
            print(f"批次 {file_idx + 1}/{len(txt_files)} 已存在，跳过")
            continue

        print(f"处理文件 {file_idx + 1}/{len(txt_files)}: {os.path.basename(txt_file)}")

        boards = []
        values = []

        with open(txt_file, 'r') as f:
            for line in f:
                line = line.strip()
                if len(line) >= 65:
                    board_str = line[:64]
                    value_str = line[65:].strip()
                    board = np.zeros((2, 8, 8), dtype=np.float32)
                    for i, c in enumerate(board_str):
                        row, col = i // 8, i % 8
                        if c == 'X':
                            board[0, row, col] = 1.0
                        elif c == 'O':
                            board[1, row, col] = 1.0
                    boards.append(board)
                    values.append(float(value_str))

        boards_arr = np.array(boards, dtype=np.float32).reshape(-1, 128)
        values_arr = np.array(values, dtype=np.float32).reshape(-1, 1)
        np.savez_compressed(batch_file, boards=boards_arr, values=values_arr)
        print(f"批次 {file_idx + 1}/{len(txt_files)} 完成: {os.path.basename(txt_file)}")

    print("预处理完成!")
    return data_dir


class OthelloDataset(Dataset):
    files_num = 0
    def __init__(self, data_dir, batch_idx=0):
        npz_files = sorted(glob.glob(os.path.join(data_dir, "train_batch_*.npz")))
        print(f"加载第 {batch_idx + 1}/{len(npz_files)} 个批次文件...")
        self.files_num = len(npz_files)
        data = np.load(npz_files[batch_idx])
        self.boards = torch.tensor(data['boards'].reshape(-1, 2, 8, 8), dtype=torch.float32)
        self.values = torch.tensor(data['values'], dtype=torch.float32)
        print(f"批次数据量: {len(self.boards)}")

    def __len__(self):
        return len(self.boards)

    def __getitem__(self, idx):
        return self.boards[idx], self.values[idx]


class OthelloDatasetTxt(Dataset):
    def __init__(self, data_path):
        self.samples = []
        with open(data_path, 'r') as f:
            for line in f:
                if len(line) >= 65:
                    value_str = line[65:].strip()
                    board = self.parse_board(line)
                    value = float(value_str)
                    self.samples.append((board, value))

    def parse_board(self, board_str):
        board = torch.zeros(2, 8, 8)
        for i in range(64):
            row,col = i // 8, i % 8
            c = board_str[i]
            if c == 'X':
                board[0, row, col] = 1.0
            elif c == 'O':
                board[1, row, col] = 1.0
        return board

    def __len__(self):
        return len(self.samples)

    def __getitem__(self, idx):
        board, value = self.samples[idx]
        return board, torch.tensor([value], dtype=torch.float32)


MODEL_CLASS = 'fcn'

def train(model_class=None):
    if model_class is None:
        # load class from MODEL_CLASS
        model_class = __import__(MODEL_CLASS).ValueNet

    data_dir = preprocess_data()
    print(f"数据目录: {data_dir}")

    model = model_class()
    model = model.to(device)
    if os.path.exists(MODEL_CLASS+'.pth'):
        model.load_state_dict(torch.load(MODEL_CLASS+'.pth'))
        print("已加载已有模型")
    else:
        print("新建模型")

    criterion = nn.MSELoss()
    optimizer = torch.optim.Adam(model.parameters(), lr=0.001)

    npz_files = sorted(glob.glob(os.path.join(data_dir, "train_batch_*.npz")))
    total_batches = len(npz_files)
    print(f"总共有 {total_batches} 个训练批次")

    for batch_idx in range(total_batches):
        print(f"\n=== 训练批次 {batch_idx + 1}/{total_batches} ===")

        dataset = OthelloDataset(data_dir, batch_idx=batch_idx)
        train_loader = DataLoader(dataset, batch_size=BATCH_SIZE, shuffle=True)

        for epoch in range(NUM_EPOCHS_TIMES):
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
            print(f"Epoch [{epoch+1}/{NUM_EPOCHS_TIMES}], Train Loss: {avg_train_loss:.4f}, Time: {epoch_time:.2f}s")

        del dataset, train_loader
        if torch.cuda.is_available():
            torch.cuda.empty_cache()
        print(f"批次 {batch_idx + 1} 训练完成，已卸载数据")

    torch.save(model.state_dict(), MODEL_CLASS+'.pth')
    print(f"模型已保存到 {MODEL_CLASS}.pth")


def evaluate(model_class=None):
    if model_class is None:
        model_class = __import__(MODEL_CLASS).ValueNet

    data_dir = r"..\Egaroucid_Train_Data\0001_egaroucid_7_5_1_lv17"
    
    model = model_class()
    model = model.to(device)
    model.load_state_dict(torch.load(MODEL_CLASS+'.pth'))
    model.eval()

    criterion = nn.MSELoss()
    
    npz_files = sorted(glob.glob(os.path.join(data_dir, "train_batch_*.npz")))
    total_batches = len(npz_files)
    print(f"总共有 {total_batches} 个批次需要推理")

    total_loss = 0.0
    total_samples = 0

    for batch_idx in range(total_batches):
        print(f"\n=== 推理批次 {batch_idx + 1}/{total_batches} ===")
        
        dataset = OthelloDataset(data_dir, batch_idx=batch_idx)
        val_loader = DataLoader(dataset, batch_size=BATCH_SIZE, shuffle=False)
        
        batch_loss = 0.0
        batch_samples = 0
        
        with torch.no_grad():
            for boards, values in val_loader:
                boards = boards.to(device)
                values = values.to(device)
                outputs = model(boards)
                loss = criterion(outputs, values)
                batch_loss += loss.item() * boards.size(0)
                batch_samples += boards.size(0)
        
        avg_batch_loss = batch_loss / batch_samples
        print(f"批次 {batch_idx + 1} MSE: {avg_batch_loss:.4f}")
        
        total_loss += batch_loss
        total_samples += batch_samples
        
        del dataset, val_loader
        if torch.cuda.is_available():
            torch.cuda.empty_cache()
        print(f"批次 {batch_idx + 1} 推理完成，已卸载数据")

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
