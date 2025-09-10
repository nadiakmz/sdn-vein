#!/bin/bash
# run_inference.sh

#source /home/nadia/Brock/Thesis/omnetpp-6.0.3/.venv/bin/activate
# Strip out the Nix environment completely
/usr/bin/env -i PATH=/usr/bin:/bin HOME="$HOME" \
    /home/nadia/Brock/Thesis/omnetpp-6.0.3/.venv/bin/python3 \
	 /home/nadia/Brock/Thesis/openflow4core-20240124/model/infer_once.py "$@"
