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

#include "GnnModel.h"
#include <cstdlib>      // std::system
#include <fstream>
#include <sstream>
#include "inet/transportlayer/contract/tcp/TCPSocket.h"
#include "inet/common/INETDefs.h"
#include "inet/controller_messages/ControllerCommand_m.h"
#include <omnetpp.h>
#include <chrono>


//#include "openflow/controllerApps/json.hpp"
//#include <nlohmann/json.hpp>
//#include "nlohmann/json.hpp" // Add this include
//using json = nlohmann::json;
namespace openflow {
Define_Module(GnnModel);
std::unordered_map<std::string, std::string> rsuToSwitchMap = {
    {"10.0.0.14", "open_flow_switch4"},
    {"10.0.0.18", "open_flow_switch4"},
    {"10.0.0.22", "open_flow_switch4"},
    {"10.0.0.26", "open_flow_switch3"},
    {"10.0.0.30", "open_flow_switch4"},
    {"10.0.0.34", "open_flow_switch3"},
    {"10.0.0.38", "open_flow_switch3"},
    {"10.0.0.42", "open_flow_switch4"},
    {"10.0.0.46", "open_flow_switch3"},
    {"10.0.0.50", "open_flow_switch2"},
    {"10.0.0.54", "open_flow_switch1"},
    {"10.0.0.58", "open_flow_switch2"}
};


void GnnModel::initialize()
    {
        // TODO - Generated method body
        inferInterval = par("inferInterval");
        pyExec        = par("pyExec").stdstringValue();
        inferScript   = par("inferScript").stdstringValue();

        modelPath     = par("modelPath").stdstringValue();
        mergedCsv     = par("mergedCsv").stdstringValue();
        rsuPositions  = par("rsuPositions").stdstringValue();
        outDecisions  = par("outDecisions").stdstringValue();
//        glob          = par("glob").stdstringValue();
        files1        = par("files1").stdstringValue();
        files2        = par("files2").stdstringValue();
        files3        = par("files3").stdstringValue();
        files4        = par("files4").stdstringValue();
        mergeScript   = par("mergeScript").stdstringValue();
        inference_outputs = par("inference_outputs").stdstringValue();
        inference_overhead = par("inference_overhead").stdstringValue();
        controller_overhead = par("controller_overhead").stdstringValue();

        ctrl_msgs_sent = 0;
//        ctrlState     = par("ctrlState").stdstringValue();


        ipToRsu["10.0.0.14"] = getModuleByPath("RSUExampleScenario.rsu[0]");
        ipToRsu["10.0.0.18"] = getModuleByPath("RSUExampleScenario.rsu[1]");
        ipToRsu["10.0.0.22"] = getModuleByPath("RSUExampleScenario.rsu[2]");
        ipToRsu["10.0.0.26"] = getModuleByPath("RSUExampleScenario.rsu[3]");

        ipToRsu["10.0.0.30"] = getModuleByPath("RSUExampleScenario.rsu[4]");
        ipToRsu["10.0.0.34"] = getModuleByPath("RSUExampleScenario.rsu[5]");
        ipToRsu["10.0.0.38"] = getModuleByPath("RSUExampleScenario.rsu[6]");
        ipToRsu["10.0.0.42"] = getModuleByPath("RSUExampleScenario.rsu[7]");

        ipToRsu["10.0.0.46"] = getModuleByPath("RSUExampleScenario.rsu[8]");
        ipToRsu["10.0.0.50"] = getModuleByPath("RSUExampleScenario.rsu[9]");
        ipToRsu["10.0.0.54"] = getModuleByPath("RSUExampleScenario.rsu[10]");
        ipToRsu["10.0.0.58"] = getModuleByPath("RSUExampleScenario.rsu[11]");

        inferTimer = new cMessage("inferTimer");
        scheduleAt(simTime() + inferInterval, inferTimer);
    }

    void GnnModel::handleMessage(cMessage *msg)
    {
        // TODO - Generated method body
        if (msg == inferTimer) {
            runInferenceOnce();
            scheduleAt(simTime() + inferInterval, inferTimer);
        } else {
            delete msg;
        }
    }

    inline void GnnModel::appendCsv(const std::string& path,
                          const std::string& header,
                          const std::string& line) {
        struct stat st;
        bool writeHeader = (stat(path.c_str(), &st) != 0 || st.st_size == 0);

        std::ofstream f(path, std::ios::app);
        if (writeHeader) f << header << "\n";
        f << line << "\n";
    }

    void GnnModel::runInferenceOnce() {
            // Option: write a lightweight CSV for the last window here if you don’t already
            // (e.g., copy/merge per-RSU logs into mergedCsv)

            // build command
        EV_INFO<< "Main Controller: Running the merge script"<<endl;
        simtime_t start = simTime();
        auto wallStart = std::chrono::steady_clock::now();

//        std::ostringstream mergeCmd;
//        mergeCmd << pyExec
//                 << " " << mergeScript
//                 << " --glob " << glob
//                 << " --out " << mergedCsv
//                 << " --dedupe minDelay"
//                 << " --rsu-positions " << rsuPositions
//                 << " --window " << inferInterval
//                 << " --simtime "<< start;

        std::stringstream command;
        command << "/home/nadia/Brock/Thesis/openflow4core-20240124/model/run_merge.sh "
//                "/home/nadia/Brock/Thesis/omnetpp-6.0.3/.venv/bin/python3 "
//                << "/home/nadia/Brock/Thesis/openflow4core-20240124/model/merge_logs.py "
//                << "--glob '/home/nadia/Brock/Thesis/SDVN/simulations/veins/results/WithBeaconing/seed0/*.csv' " // Important: Quote the glob pattern
                << "--files "<< files1<< " "<< files2<< " "<< files3<< " "<< files4
                << " --out "<< mergedCsv
                << " --dedupe minDelay"
                << " --rsu-positions " << rsuPositions
                << " --window " <<inferInterval
                << " --simtime "<< start
                << " > /tmp/python_output.log 2>&1"; // This is the magic line

        EV_INFO << "[Controller] Calling: " << command.str() << endl;
        int rc_test = system(command.str().c_str());

        if (rc_test != 0) {
            EV_WARN << "[Controller] merge_logs failed, rc=" << rc_test << ". Check /tmp/python_output.log for details." << endl;
        }

//        EV_INFO << "[Controller] Calling: " << mergeCmd.str() << "\n";
//        int rcMerge = std::system(mergeCmd.str().c_str());
//        if (rcMerge != 0) {
//            EV_WARN << "[Controller] merge_logs failed, rc=" << rcMerge << "\n";
//            return;
//        }

        EV_INFO<< "Main Controller: Running the Inference script"<<endl;
        std::ostringstream cmd;
        cmd << "/home/nadia/Brock/Thesis/openflow4core-20240124/model/run_inferenece.sh "
//                pyExec << " " << inferScript
                << " --model " << modelPath
                << " --merged " << mergedCsv
                << " --rsu-positions " << rsuPositions
                << " --simtime " << SIMTIME_DBL(simTime())
                << " --out-decisions " << outDecisions
                << " --T " << inferInterval
                << " --out-inference "<< inference_outputs
                << " --out-inference-overhead "<< inference_overhead
                << " > /tmp/python_output.log 2>&1"; // This is the magic line


        auto wallAfterPy = std::chrono::steady_clock::now();

        EV_INFO << "[Controller] Calling: " << cmd.str() << "\n";
        int rc = std::system(cmd.str().c_str());
        if (rc != 0) {
            EV_WARN << "[Controller] Inference process returned non-zero: " << rc << "\n";
            return;
        }

            // read decisions json
        std::ifstream f(outDecisions);
        if (!f.good()) {
            EV_WARN << "[Controller] Could not open decisions file: " << outDecisions << "\n";
            return;
        }
//        json j; f >> j;

        applyDecisions(outDecisions);
        auto wallAfterApply = std::chrono::steady_clock::now();

        // compute times (wall clock, not simtime)
        double t_launch_ms = std::chrono::duration<double, std::milli>(wallAfterPy - wallStart).count();
        double t_apply_ms  = std::chrono::duration<double, std::milli>(wallAfterApply - wallAfterPy).count();



        // log
        std::ostringstream oss;
        oss << SIMTIME_DBL(simTime()) << "," << t_launch_ms << "," << t_apply_ms << "," << ctrl_msgs_sent;
        appendCsv(controller_overhead,
                  "simTime,t_launch_ms,t_apply_ms,ctrl_msgs_sent",
                  oss.str());

    }
    void GnnModel::applyDecisions(const std::string& path) {
        std::ifstream f(path);
        if (!f.good()) {
                EV_WARN << "[Controller] Could not open decisions file: " << path << "\n";
                return;
        }
        static int decisionId = 0;
        std::string line;
           while (std::getline(f, line)) {
               if (line.rfind("t=", 0) == 0) {
                   EV_INFO << "[Controller] Simulation time " << line << "\n";
                   continue;
               }

               std::string rsuIP;
               if (line.find("RSU=") != std::string::npos) {
                   auto pos = line.find(" ");
                   rsuIP = line.substr(4, pos - 4);  // extract after "RSU="
               }

               if (line.find("ATTACH:") != std::string::npos) {
                   std::vector<int> vehicles;
                   std::istringstream iss(line.substr(line.find("ATTACH:") + 7));
                   int vid;
                   while (iss >> vid) {
                       vehicles.push_back(vid);
                   }
                   sendAttachBatch(vehicles, rsuIP);
               }

               else if (line.find("REROUTE:") != std::string::npos) {
                   std::string rest = line.substr(line.find("REROUTE:") + 8);
                   int vid;
                   std::string tgt;
                   char arrow;
                   std::istringstream iss(rest);
                   iss >> vid >> arrow >> tgt;  // e.g. "104->10.0.0.3"
                   sendReroute(vid, tgt);
           }
           }
    }

//        auto decisions = j["decisions"];
//        for (const auto& d : decisions) {
//                std::string rsuIP = d.value("rsuIP", "");
//                EV_INFO << "[Controller] Decisions for RSU " << rsuIP << "\n";
//
//                // Handle detach
//                if (d.contains("detach")) {
//                    for (auto v : d["detach"]) {
//                        EV_INFO << "  Detach vehicle " << v << " from " << rsuIP << "\n";
//                        // TODO: implement actual detach in your RSU/vehicle modules
//                    }
//                }
//
//                // Handle attach
//                if (d.contains("attach")) {
//                    std::vector<int> vehicles = d["attach"].get<std::vector<int>>();
//                    sendAttachBatch(vehicles, rsuIP);
//                }
//                // Handle reroute
//                if (d.contains("reroute")) {
//                    for (auto r : d["reroute"]) {
//                        int v = r.value("vehicle", -1);
//                        std::string tgt = r.value("toRSU", "");
//                        EV_INFO << "  Reroute vehicle " << v << " via RSU " << tgt << "\n";
//                        sendReroute(v,tgt);
//                        // TODO: update flow tables or routes here
//                    }
//                }
//            }


        // helper to print vectors
    static std::string vecToStr(const std::vector<int>& v) {
        std::ostringstream oss; oss << "[";
        for (size_t i=0;i<v.size();++i) { if (i) oss<<","; oss<<v[i]; }
        oss << "]";
        return oss.str();
    }

    void GnnModel::sendAttachBatch(const std::vector<int>& vehicles, const std::string& targetRSU) {
        if (vehicles.empty()) return;
        auto it = ipToRsu.find(targetRSU);
            if (it == ipToRsu.end()) {
                EV_WARN << "[Controller] Unknown RSU IP " << targetRSU << "\n";
                return;
            }

        auto *cmdMsg = new ControllerCommand("attachBatch");
        cmdMsg->setCmd("attach");
        cmdMsg->setTargetRSU(targetRSU.c_str());
        cmdMsg->setVehicleIDsArraySize(vehicles.size());
        for (size_t i = 0; i < vehicles.size(); i++) {
            cmdMsg->setVehicleIDs(i, vehicles[i]);
        }


        sendDirect(cmdMsg, it->second, "controlIn");
        EV_INFO << "[Controller] Sent attach batch of " << vehicles.size()
                    << " vehicles to RSU " << targetRSU << "\n";
        ctrl_msgs_sent++;
    }

    void GnnModel::sendReroute(int vehicleID, const std::string& targetIP) {
        auto it = ipToRsu.find(targetIP);
        if (it == ipToRsu.end()) {
            EV_WARN << "[Controller] Unknown RSU IP " << targetIP << "\n";
            return;
        }

        auto *cmdMsg = new ControllerCommand("reroute");
        cmdMsg->setCmd("reroute");
        cmdMsg->setTargetRSU(targetIP.c_str());
        std::vector<int> vehicles = { vehicleID };
        cmdMsg->setVehicleIDsArraySize(vehicles.size());
        for (size_t i = 0; i < vehicles.size(); i++) {
            cmdMsg->setVehicleIDs(i, vehicles[i]);
        }


        sendDirect(cmdMsg, it->second, "controlIn");
        EV_INFO << "[Controller] Sent reroute for vehicle "
                << vehicleID << " to RSU " << targetIP << "\n";
        ctrl_msgs_sent++;
    }



//    // ---------- “apply” helpers (stubbed; wire them to your modules) ----------
//    void sendSetCHToRSU(const std::string& rsuIP, int newCH) {
//        // Option 1: if your controller knows RSU module paths by IP, find RSU module and send cMessage
//        // Option 2: record CH mapping in a table, and your RSU pulls (polls) from controller via a query
//        // Option 3: add a small .msg type and send on a control gate.
//        EV_INFO << "[Controller] Set CH " << newCH << " at RSU " << rsuIP << "\n";
//        // TODO: implement according to your topology (controlPlane gates or direct)
//
//        auto it = rsuToSwitchMap.find(rsuIP);
//        if (it == rsuToSwitchMap.end()) {
//                EV_WARN << "[Controller] No mapping found for RSU " << rsuIP << "\n";
//                return;
//            }
//        std::string swId = it->second;
//
//            // 2. Find TCP socket for that switch
//        TCPSocket* sock = findSocketForChassisId(swId);
//            if (!sock) {
//                EV_WARN << "[Controller] No socket found for switch " << swId
//                        << " (RSU " << rsuIP << ")\n";
//                return;
//            }
//
//            // 3. Build a custom OpenFlow Experimenter (or vendor-specific) message
//            // NOTE: You can also use OFP_Flow_Mod here if you want to install rules.
////            OFP_Experimenter* msg = new OFP_Experimenter("CH_Update");
////            msg->setKind(TCP_C_SEND);
//
//            // Example payload: store RSU IP + CH vehicle ID in experimenter fields
////            msg->setExperimenter(12345);  // your vendor ID
////            std::string payload = "RSU=" + rsuIP + ";CH=" + std::to_string(newCH);
////            msg->setByteLength(8 + payload.size()); // header + payload size
//
////            EV_INFO << "[Controller] Sending CH update: RSU " << rsuIP
////                    << " -> newCH=" << newCH << " via switch " << swId << "\n";
//
//
////
//        CH_Update *msg = new CH_Update("CH_Update");
//        msg->setKind(TCP_C_SEND);
////
////           // Fill OpenFlow header (mark it as vendor/experimenter type)
//        ofp_header hdr = msg->getHeader();
//        hdr.version = OFP_VERSION;
//        hdr.type = OFPT_VENDOR;   // vendor = experimenter type
//        msg->setHeader(hdr);
////
////           // Fill payload
//        msg->setRsuIP(rsuIP.c_str());
//        msg->setNewCH(newCH);
//        std::string payload = "RSU=" + rsuIP + ";CH=" + std::to_string(newCH);
//        msg->setByteLength(8 + payload.size());
////
////           // Debug
////        EV_INFO << "[Controller] Sending CH update: RSU=" << rsuIP << " newCH=" << newCH << endl;
////
////        TCPSocket* sock = findSocketForChassisId("switchID_here");
//
//        controller->sendPacketOut(msg,sock);
//           // Send on control channel to switch
////           emit(PacketOutSignalId, msg);
////           socket->send(msg);
//
//
//
//
//    }
//

    void GnnModel::finish() {
        cancelAndDelete(inferTimer);
    }
}
