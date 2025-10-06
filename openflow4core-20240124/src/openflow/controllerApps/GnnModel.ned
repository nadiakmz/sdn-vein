//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package openflow.controllerApps;

//
// TODO auto-generated module
//
simple GnnModel like AbstractControllerApp
{
    parameters:
        @class(openflow::GnnModel);
        @display("i=block/app");
        string controller_decisions = default("default_controller_decisions.csv");
        
        int priority = default(1);
        double inferInterval @unit(s) = default(5s); //10s
        string pyExec = default("python3");
        string inferScript = default("../../../model/infer_once.py");
        string modelPath = default("../../../models/stgnn.pth");
        string mergedCsv = default(""); //it should be changed
        string rsuPositions = default("../../../model/rsu_positions.json");
        string outDecisions = default("../../../model/decisions.csv");
        string inference_overhead = default("../../../model/inference_overhead.csv");
        string inference_outputs = default("../../../model/inference_outputs.csv");
        string controller_overhead = default("../../../model/controller_overhead.csv");
        string clusters_output = default("../../../model/clusters_output.csv");
//        string glob = default("");
		string files1 = default("");
		string files2 = default("");
		string files3 = default("");
		string files4 = default("");
        string mergeScript = default("");
//        int ctrl_msgs_sent = 0;
//        string ctrlState = default("../../../controller_state.json");
//    gates:
        // hook up to switches/RSUs as you need; not required for file I/O workflow
}
