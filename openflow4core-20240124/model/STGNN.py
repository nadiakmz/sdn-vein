# D: model.py
import torch
import torch.nn as nn
import torch.nn.functional as F
from torch_geometric.nn import GATConv

class SnapshotGNN(nn.Module):
    def __init__(self, in_dim, hid_dim=64, out_dim=32, heads=2):
        super().__init__()
        self.gat = GATConv(in_dim, hid_dim, heads=heads, concat=False)
        self.lin = nn.Linear(hid_dim, out_dim)

    def forward(self, x, edge_index):
        # x: [N, F]
        h = self.gat(x, edge_index)
        h = F.elu(self.lin(h))
        return h  # [N, out_dim]

class STGNN(nn.Module):
    def __init__(self, in_dim, gnn_hid=64, gnn_out=64, rnn_hid=64):
        super().__init__()
        self.snap_gnn = SnapshotGNN(in_dim, hid_dim=gnn_hid, out_dim=gnn_out)
        self.rnn = nn.GRU(input_size=gnn_out, hidden_size=rnn_hid, batch_first=True)
        # heads
        self.stab_head = nn.Linear(rnn_hid, 1)   # sigmoid BCE
        self.lon_head = nn.Linear(rnn_hid, 1)    # regression
        self.load_head = nn.Linear(rnn_hid, 1)   # RSU load (if used per-RSU)
    def forward(self, X_seq, edge_index_seq):
        # X_seq: list length T of [N, F] tensors; edge_index_seq list length T of edge_index for each snapshot
        H_seq = []
        for x, ei in zip(X_seq, edge_index_seq):
            h = self.snap_gnn(x, ei)   # [N, outdim]
            H_seq.append(h)
        H = torch.stack(H_seq, dim=1)  # [N, T, outdim]
        # run RNN per node: need to swap to batch order
        out, _ = self.rnn(H)  # [N, T, rnn_hid]
        last = out[:, -1, :]  # [N, rnn_hid]
        stab = torch.sigmoid(self.stab_head(last)).squeeze(-1)
        lon = self.lon_head(last).squeeze(-1)
        load = self.load_head(last).squeeze(-1)
        return stab, lon, load
