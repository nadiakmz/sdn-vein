#!/usr/bin/env python3
import argparse
import glob
import os
import json
import pandas as pd
from math import hypot


rsu_to_district = {
    "10.0.0.42": 3,
    "10.0.0.46": 3,
    "10.0.0.50": 3,
    "10.0.0.26": 2,
    "10.0.0.54": 3,
    "10.0.0.30": 2,
    "10.0.0.34": 2,
    "10.0.0.58": 3,
    "10.0.0.38": 2,
    "10.0.0.18": 1,
    "10.0.0.14": 0,
    "10.0.0.22": 1
}

def load_all(files):
    # pattern = csv_glob.strip("'").strip('"')
    # files = sorted(glob.glob(pattern))
    dfs = []
    for f in files:
        print(f"[merge_logs] Checking file: {f}")
        print(f"  size = {os.path.getsize(f)} bytes")
        if os.path.getsize(f) == 0:
            # completely empty file
            continue
        try:
            df = pd.read_csv(f)
        except pd.errors.EmptyDataError:
            continue
        if df.empty:
            continue
        # keep only if real rows beyond header
        if len(df) == 0:
            continue
        # df = pd.read_csv(f)
        df['source_file'] = os.path.basename(f)
        df["districtID"] = df["rsuIP"].map(rsu_to_district)
        dfs.append(df)
    if not dfs:
        raise RuntimeError("No non-empty CSV files among: " + ", ".join(files))
    return pd.concat(dfs, ignore_index=True)

def nearest_rsu(row, rsu_positions):
    x,y = row['senderPosX'], row['senderPosY']
    best = None
    bestd = 1e18
    for k,(rx,ry) in rsu_positions.items():
        d = hypot(x-rx, y-ry)
        if d < bestd:
            bestd = d; best = int(k)
    return best

def dedupe(df, rsu_positions=None, dedupe_by='minDelay'):
    df['simTime_round'] = df['simTime'].round(3)  # ms resolution

    if dedupe_by == 'minDelay':
        df_sorted = df.sort_values(
            by=['vehicleID','simTime_round','packetSize','endToEndDelay']
        )
        df_nd = df_sorted.drop_duplicates(
            subset=['vehicleID','simTime_round','packetSize'], keep='first'
        )
        return df_nd.drop(columns=['simTime_round'])

    elif dedupe_by == 'nearestRSU':
        if rsu_positions is None:
            raise ValueError("rsu_positions required for nearestRSU dedupe")
        df['nearest'] = df.apply(lambda r: nearest_rsu(r, rsu_positions), axis=1)
        df['districtID'] = df['districtID'].astype(int)
        df2 = df[df['districtID'] == df['nearest']]
        if df2.empty:
            df2 = df.sort_values('endToEndDelay').drop_duplicates(
                subset=['vehicleID','simTime_round','packetSize'])
        else:
            df2 = df2.sort_values('endToEndDelay').drop_duplicates(
                subset=['vehicleID','simTime_round','packetSize'])
        return df2.drop(columns=['simTime_round','nearest'])

    else:
        raise ValueError("unknown dedupe_by")


def main():
    ap = argparse.ArgumentParser()
    
    # ap.add_argument("--glob", required=True, help="Glob pattern for input CSVs")
    ap.add_argument("--files", nargs="+", required=True, help="List of CSV files to merge")
    ap.add_argument("--out", required=True, help="Output merged CSV")
    ap.add_argument("--dedupe", choices=["minDelay","nearestRSU"], default="minDelay")
    ap.add_argument("--rsu-positions", default="", help="Optional RSU positions JSON")
    ap.add_argument("--window", type=float, default=0,
                    help="Keep only the last T seconds of logs (0 = keep all)")
    ap.add_argument("--simtime", type=float, help="Controller current simTime (s). If omitted, uses max(simTime) in merged logs")

    args = ap.parse_args()

    df = load_all(args.files)
    if df.empty:
        print(f"[merge_logs] Skipping merge: all input CSVs are empty")
        return

    rsu_positions = None
    if args.dedupe == "nearestRSU" and args.rsu_positions:
        with open(args.rsu_positions) as f:
            rsu_positions = json.load(f)

    df2 = dedupe(df, rsu_positions=rsu_positions, dedupe_by=args.dedupe)
    df2 = df2.sort_values('simTime')
    if args.window > 0 and not df2.empty:
        t_max = df2['simTime'].max()
        if args.simtime  and args.simtime > 0:
            t_max = args.simtime;
        
        df2 = df2[df2['simTime'] >= t_max - args.window]
        df2 = df2[df2['simTime'] <= t_max]
    df2.to_csv(args.out, index=False, mode="w")

    # print(f"[merge_logs] Wrote {args.out}, rows={len(df2)} from {args.glob}")
    print(f"[merge_logs] Wrote {args.out}, rows={len(df2)} from {len(args.files)} input files")


if __name__ == '__main__':
    main()
