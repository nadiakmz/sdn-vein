#!/usr/bin/env python3
# infer_once.py
import argparse, json, os, math, time
import numpy as np
import pandas as pd
import torch
from sklearn.neighbors import NearestNeighbors

# ===== import your model exactly as trained =====
from STGNN import STGNN


# ----------------- helpers -----------------
def load_rsu_positions(path):
    """Supports {'10.0.0.14':[x,y], ...} AND {'10.0.0.14':{'x':..,'y':..}}."""
    with open(path) as f:
        raw = json.load(f)
    pos = {}
    for k, v in raw.items():
        if isinstance(v, (list, tuple)) and len(v) == 2:
            pos[k] = {"x": float(v[0]), "y": float(v[1])}
        elif isinstance(v, dict) and "x" in v and "y" in v:
            pos[k] = {"x": float(v["x"]), "y": float(v["y"])}
        else:
            raise ValueError(f"Bad RSU position for {k}: {v}")
    return pos

def bucketize(df, dt):
    df = df.copy()
    df["tbin"] = (df["simTime"] // dt).astype(int)
    return df

def latest_per_vehicle(df_t):
    # last record per vehicle in the bin
    return df_t.sort_values("simTime").groupby("vehicleID").tail(1)

def build_edges_knn(positions, k=6, dist_threshold=300.0):
    N = len(positions)
    if N == 0:
        return np.empty((2,0), dtype=int)
    nbrs = NearestNeighbors(n_neighbors=min(k+1, N)).fit(positions)
    distances, indices = nbrs.kneighbors(positions)
    rows = []
    for i in range(N):
        for jidx, dist in zip(indices[i,1:], distances[i,1:]):  # skip self
            if dist <= dist_threshold:
                rows.append((i, int(jidx)))
    if not rows:
        return np.empty((2,0), dtype=int)
    return np.array(rows).T

def load_model(model_path, in_dim):
    # TorchScript .pt or state_dict .pth
    if model_path.endswith(".pt"):
        m = torch.jit.load(model_path, map_location="cpu")
        m.eval()
        return m
    m = STGNN(in_dim)
    sd = torch.load(model_path, map_location="cpu")
    m.load_state_dict(sd)
    m.eval()
    return m


# -------- snapshot builder for inference (NO labels) --------
def build_snapshot_from_logs(
    df, rsu_pos, dt=1.0, T=10, k=6, dist=300.0, simtime=None
):
    """
    Build an inference snapshot (no labels) from merged logs.

    Required df columns:
      simTime, vehicleID, senderPosX, senderPosY, senderSpeed, senderDirection, packetSize, rsuIP
    """
    df = bucketize(df, dt)
    if df.empty:
        raise ValueError("Empty dataframe")

    t_now = int(simtime // dt) if simtime is not None else int(df["tbin"].max())
    seq_ts = list(range(t_now - T + 1, t_now + 1))
    if min(df["tbin"]) > seq_ts[0]:
        raise ValueError("Not enough history for T bins")

    # Vehicles present at t_now define vehicle nodes
    df_latest = latest_per_vehicle(df[df["tbin"] == t_now])
    veh_ids = df_latest["vehicleID"].astype(int).tolist()

    # RSU nodes (strings)
    rsu_ids = list(rsu_pos.keys())
    vid_list = veh_ids + rsu_ids
    N = len(vid_list)
    if N == 0:
        raise ValueError("No nodes to build snapshot")

    # Build features & positions for T bins
    X_seq, pos_seq = [], []
    for tt in seq_ts:
        df_tt = df[df["tbin"] == tt]
        df_map = {int(r.vehicleID): r for _, r in df_tt.iterrows()}
        feats, poss = [], []
        for vid in vid_list:
            if isinstance(vid, int):  # vehicle
                if vid in df_map:
                    r = df_map[vid]
                    x = float(r.senderPosX); y = float(r.senderPosY)
                    spd = float(r.senderSpeed)
                    hdg = float(r.senderDirection)
                    pkt = float(r.packetSize)
                else:
                    x=y=spd=hdg=pkt=0.0
                feats.append([spd, math.cos(math.radians(hdg)), math.sin(math.radians(hdg)), pkt])
                poss.append((x, y))
            else:  # RSU
                p = rsu_pos[vid]
                feats.append([0.0, 0.0, 0.0, 0.0])
                poss.append((p["x"], p["y"]))
        X_seq.append(np.array(feats, dtype=float))
        pos_seq.append(np.array(poss, dtype=float))

    # Edges for latest snapshot:
    rows = []
    # Vehicle <-> RSU (undirected) from t_now
    for row in df_latest.itertuples():
        v = int(row.vehicleID)
        rsu = getattr(row, "rsuIP", None)
        if rsu is None:
            continue
        if v in veh_ids and rsu in rsu_pos:
            v_idx = vid_list.index(v)
            r_idx = vid_list.index(rsu)
            rows.append((v_idx, r_idx))
            rows.append((r_idx, v_idx))
    # Vehicle-vehicle proximity edges
    pos_t_veh = pos_seq[-1][:len(veh_ids)]
    edge_vv = build_edges_knn(pos_t_veh, k=k, dist_threshold=dist)
    rows.extend(edge_vv.T.tolist())
    edge_index = np.array(rows).T if rows else np.empty((2,0), dtype=int)

    # Also return df_latest for controller logic (who is attached to which RSU now)
    return {
        "t": int(t_now),
        "vid_list": vid_list,
        "X_seq": [x.tolist() for x in X_seq],
        "pos_seq": [p.tolist() for p in pos_seq],
        "edge_index": edge_index.tolist(),
        "meta": {"has_labels": False},
        "_df_latest": df_latest[["vehicleID","rsuIP"]].copy()  # transient (not written to JSON)
    }


# -------- decisions policy --------
def euclid(a, b):
    return math.hypot(a[0]-b[0], a[1]-b[1])

def plan_decisions(
    vid_list, pred_stab, pred_load, pred_lon, df_latest, rsu_pos,
    t_now,
    # max_load_pps=80.0,
    # min_stability=0.60,
    # max_latency_ms=100.0,
    # rsu_nei_dist=400.0,
    
    max_load_pps = 10.0,
    min_stability = 0.60,
    max_latency_ms = 0.001,
    rsu_nei_dist = 400.0

):
    """
    Returns a list of decision dicts, one per RSU observed in this tick.

    Decisions are:
      - reassign vehicles if stability is low
      - reroute vehicles if latency > SLA
      - offload vehicles if RSU is overloaded
    """

    idx_map = {vid: i for i, vid in enumerate(vid_list)}
    veh_nodes = [vid for vid in vid_list if isinstance(vid, int)]
    rsu_nodes = [vid for vid in vid_list if isinstance(vid, str)]

    # Current vehicle → RSU attachments
    attach_map = {}
    for row in df_latest.itertuples():
        v = int(row.vehicleID); r = row.rsuIP
        attach_map.setdefault(r, []).append(v)

    # RSU positions
    rsu_xy = {r: (rsu_pos[r]["x"], rsu_pos[r]["y"]) for r in rsu_nodes}

    def nearest_rsus(r, k=3, within=rsu_nei_dist):
        """Find neighbor RSUs by geo distance."""
        if r not in rsu_xy: return []
        rx, ry = rsu_xy[r]
        dsts = [(other, math.hypot(rx - rsu_xy[other][0], ry - rsu_xy[other][1]))
                for other in rsu_nodes if other != r]
        dsts.sort(key=lambda x: x[1])
        return [o for (o, d) in dsts if d <= within][:k]

    # Predictions mapped
    veh_pred_stab = {v: float(pred_stab[idx_map[v]]) for v in veh_nodes if v in idx_map}
    veh_pred_lat  = {v: float(pred_lon[idx_map[v]]) for v in veh_nodes if v in idx_map}
    rsu_pred_load = {r: float(pred_load[idx_map[r]]) for r in rsu_nodes if r in idx_map}

    decisions = []

    for r in rsu_nodes:
        vehs_here = attach_map.get(r, [])
        detach, attach, reroute = [], [], []

        # --- 1. Stability check ---
        for v in vehs_here:
            if veh_pred_stab.get(v, 1.0) < min_stability:
                # try moving to nearest stable RSU
                neighbors = nearest_rsus(r)
                if neighbors:
                    tgt = min(neighbors, key=lambda rr: rsu_pred_load.get(rr, 0.0))
                    detach.append(v)
                    attach.append(v)
                    reroute.append({"vehicle": v, "toRSU": tgt})

        # --- 2. Latency check ---
        for v in vehs_here:
            if veh_pred_lat.get(v, 0.0) > max_latency_ms:
                neighbors = nearest_rsus(r)
                if neighbors:
                    tgt = min(neighbors, key=lambda rr: rsu_pred_load.get(rr, 0.0))
                    reroute.append({"vehicle": v, "toRSU": tgt})

        # --- 3. RSU Overload check ---
        overload = rsu_pred_load.get(r, 0.0) > max_load_pps
        if overload and vehs_here:
            neighbors = nearest_rsus(r)
            if neighbors:
                tgt = min(neighbors, key=lambda rr: rsu_pred_load.get(rr, 0.0))
                # offload ~20% lowest-stability vehicles
                scored = [(veh_pred_stab.get(v, 1.0), v) for v in vehs_here]
                scored.sort(key=lambda x: x[0])
                nmove = max(1, int(0.2 * len(scored)))
                to_move = [v for _, v in scored[:nmove]]
                detach.extend(to_move)
                attach.extend(to_move)
                for v in to_move:
                    reroute.append({"vehicle": v, "toRSU": tgt})

        decisions.append({
            "rsuIP": r,
            "detach": detach,
            "attach": attach,
            "reroute": reroute
        })

    return decisions

def append_with_header(path, header, line):
    write_header = not os.path.exists(path) or os.path.getsize(path) == 0
    with open(path, "a") as f:
        if write_header:
            f.write(header + "\n")
        f.write(line + "\n")


# ----------------- main -----------------
def main():
    t0 = time.time()
    
    ap = argparse.ArgumentParser()
    # Either pass a ready snapshot.json or raw logs + rsu positions:
    ap.add_argument("--snapshot", help="Existing snapshot JSON (vehicles+RSUs nodes, no labels)")
    ap.add_argument("--merged", help="Merged CSV with per-packet logs")
    ap.add_argument("--rsu-positions", help="RSU positions JSON (e.g., {'10.0.0.14':[x,y],...})")
    ap.add_argument("--dt", type=float, default=1.0)
    ap.add_argument("--T", type=int, default=10)
    ap.add_argument("--k", type=int, default=6)
    ap.add_argument("--dist", type=float, default=300.0)
    ap.add_argument("--simtime", type=float, help="Controller current simTime (s). If omitted, uses max(simTime) in merged logs")

    # Model + outputs
    ap.add_argument("--model", required=True, help=".pth (state_dict) or .pt (TorchScript)")
    ap.add_argument("--out-preds", default="", help="Optional: write raw predictions JSON")
    ap.add_argument("--out-decisions", required=True, help="Write controller decisions JSON")
    ap.add_argument("--out-snapshot", default="", help="Optional: write the snapshot used (for debugging)")
    ap.add_argument("--out-inference-overhead", default="", help="Optional: write inference overhead csv")
    ap.add_argument("--out-inference", default="", help="Optional: write raw predictions csv")
    # ap.add_argument("--state", default="controller_state.json", help="Path to persist controller state (CH, lastSwitch per RSU)")

    # Policy params
    ap.add_argument("--max-load-pps", type=float, default=80.0)
    ap.add_argument("--min-ch-stab", type=float, default=0.60)
    ap.add_argument("--cooldown-s", type=float, default=20.0)
    ap.add_argument("--rsu-nei-dist", type=float, default=400.0)
    args = ap.parse_args()

    # Build or load snapshot (no labels)
    if args.snapshot:
        with open(args.snapshot) as f:
            snap = json.load(f)
        # No df_latest in JSON form; build attach map from positions alone is unreliable.
        # For policy, we need current attachments; when calling with --snapshot, skip CH/offload
        df_latest = None
        print("[infer_once] Warning: --snapshot provided; decisions may be limited without attachments.")
    else:
        if not (args.merged and args.rsu_positions):
            raise SystemExit("Provide either --snapshot OR both --merged and --rsu-positions")
        df = pd.read_csv(args.merged)
        rsu_pos = load_rsu_positions(args.rsu_positions)
        snap = build_snapshot_from_logs(
            df, rsu_pos, dt=args.dt, T=args.T, k=args.k, dist=args.dist, simtime=args.simtime
        )
        df_latest = snap.pop("_df_latest")  # extract & remove helper df
    t1 = time.time()
    # Tensors
    vid_list = snap["vid_list"]
    X_seq = [torch.tensor(arr, dtype=torch.float32) for arr in snap["X_seq"]]
    ei = torch.tensor(snap["edge_index"], dtype=torch.long) if len(snap["edge_index"])>0 else torch.empty((2,0), dtype=torch.long)
    ei_seq = [ei]*len(X_seq)

    in_dim = len(X_seq[0][0])
    model = load_model(args.model, in_dim)
    model.eval()

    with torch.no_grad():
        pred_stab, pred_lon, pred_load = model(X_seq, ei_seq)

    # Optional: persist snapshot used
    if args.out_snapshot:
        with open(args.out_snapshot, "w") as f:
            json.dump(snap, f, indent=2)
    
    t2 = time.time()
    t_build_ms = (t1 - t0) * 1000
    t_infer_ms = (t2 - t1) * 1000
    
    append_with_header(args.out_inference_overhead,
        "simTime,t_build_snapshot_ms,t_infer_ms",
        f"{snap['t']},{t_build_ms:.3f},{t_infer_ms:.3f}")
    # Optional: raw predictions
    if args.out_preds:
        raw = {
            "t": snap["t"],
            "vid_list": vid_list,
            "pred_stab": pred_stab.tolist(),
            "pred_lon": pred_lon.tolist(),
            "pred_load": pred_load.tolist()
        }
        with open(args.out_preds, "w") as f:
            json.dump(raw, f, indent=2)
    
    for i, vid in enumerate(snap["vid_list"]):
        nodeType = "veh" if isinstance(vid, int) else "rsu"
        nodeId   = str(vid)
        append_with_header(args.out_inference,
            "simTime,nodeType,nodeId,pred_stab,pred_latency_ms,pred_load_pps",
            f"{snap['t']},{nodeType},{nodeId},{pred_stab[i]:.4f},{pred_lon[i]:.4f},{pred_load[i]:.4f}")
    

    # Decisions (needs attachments from current bin)
    decisions = []
    # new_state = {}
    if df_latest is not None:
        rsu_pos = load_rsu_positions(args.rsu_positions)

        # load prior state (persist CH per RSU across ticks)
        # if os.path.exists(args.state):
        #     try:
        #         with open(args.state) as f:
        #             state = json.load(f)
        #     except Exception:
        #         state = {}
        # else:
        #     state = {}

        decisions = plan_decisions(
            vid_list=vid_list,
            pred_stab=pred_stab.numpy(),
            pred_load=pred_load.numpy(),
            pred_lon=pred_lon.numpy(), 
            df_latest=df_latest,
            rsu_pos=rsu_pos,
            t_now=snap["t"],
            max_load_pps=args.max_load_pps,
            min_stability=args.min_ch_stab
            # cooldown_s=args.cooldown_s
        )

        # persist state
        # with open(args.state, "w") as f:
        #     json.dump(new_state, f, indent=2)

    # # Write decisions JSON for OMNeT++ controller to consume
    # out = {
    #     "t": snap["t"],
    #     "decisions": decisions,
    #     "notes": {
    #         "in_dim": in_dim,
    #         "nodes": len(vid_list),
    #         "policy": {
    #             "max_load_pps": args.max_load_pps,
    #             "min_ch_stab": args.min_ch_stab,
    #             "cooldown_s": args.cooldown_s,
    #             "rsu_neighbor_within_m": args.rsu_nei_dist
    #         }
    #     }
    # }
    # with open(args.out_decisions, "w") as f:
    #     json.dump(out, f, indent=2)
    #
    # with open(args.out_decisions, "w") as f:
    #     f.write(f"t={snap['t']}\n")
    #     for d in decisions:
    #         rsu = d["rsuIP"]
    #
    #         # Attach batch
    #         if d["attach"]:
    #             f.write(f"RSU={rsu} ATTACH: {' '.join(map(str, d['attach']))}\n")
    #
    #         # Reroute (one per vehicle)
    #         for r in d["reroute"]:
    #             v = r["vehicle"]
    #             tgt = r["toRSU"]
    #             f.write(f"RSU={rsu} REROUTE: {v}->{tgt}\n")
    #
    nextDecisionId = 0
    for d in decisions:
        print(d)
        rsuIP  = d["rsuIP"]
        # ATTACH
        for v in d.get("attach", []):
            # print(f"[attach]: {snap['t']},{nextDecisionId},attach,{v},,{rsuIP}")
            append_with_header(args.out_decisions,
                "simTime,decisionId,type,vehicleID,srcRSU,dstRSU",
                f"{snap['t']},{nextDecisionId},attach,{v},,{rsuIP}")
            nextDecisionId += 1
        # REROUTE
        if d.get("reassignTo", ""):
            tgt = d["reassignTo"]
            for v in d.get("detach", []):
                # print(f"{snap['t']},{nextDecisionId},reroute,{v},{rsuIP},{tgt}")
                append_with_header(args.out_decisions,
                    "simTime,decisionId,type,vehicleID,srcRSU,dstRSU",
                    f"{snap['t']},{nextDecisionId},reroute,{v},{rsuIP},{tgt}")
                nextDecisionId += 1


    # Also print a brief summary for logs
    print(f"[infer_once] t={snap['t']} nodes={len(vid_list)} -> wrote {len(decisions)} decisions to {args.out_decisions}")


if __name__ == "__main__":
    main()
