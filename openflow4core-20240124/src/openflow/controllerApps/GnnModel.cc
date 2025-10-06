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
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "inet/transportlayer/contract/tcp/TCPSocket.h"

#include "inet/common/INETDefs.h"
#include "inet/controller_messages/ControllerCommand_m.h"
#include <omnetpp.h>
#include <chrono>
#include <sys/stat.h>


//#include "openflow/controllerApps/json.hpp"
//#include <nlohmann/json.hpp>
//#include "nlohmann/json.hpp" // Add this include
//using json = nlohmann::json;
namespace openflow {
Define_Module(GnnModel);
std::unordered_map<std::string, std::string> rsuToSwitchMap = {
    {"10.0.0.42", "open_flow_switch4"},
    {"10.0.0.46", "open_flow_switch4"},
    {"10.0.0.50", "open_flow_switch4"},
    {"10.0.0.26", "open_flow_switch3"},
    {"10.0.0.54", "open_flow_switch4"},
    {"10.0.0.30", "open_flow_switch3"},
    {"10.0.0.34", "open_flow_switch3"},
    {"10.0.0.58", "open_flow_switch4"},
    {"10.0.0.38", "open_flow_switch3"},
    {"10.0.0.18", "open_flow_switch2"},
    {"10.0.0.14", "open_flow_switch1"},
    {"10.0.0.22", "open_flow_switch2"}
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
        clusters_output = par("clusters_output").stdstringValue();

        ctrl_msgs_sent = 0;
//        ctrlState     = par("ctrlState").stdstringValue();


        ipToRsu["10.0.0.42"] = getModuleByPath("RSUExampleScenario.rsu[0].appl");
        ipToRsu["10.0.0.46"] = getModuleByPath("RSUExampleScenario.rsu[1].appl");
        ipToRsu["10.0.0.50"] = getModuleByPath("RSUExampleScenario.rsu[2].appl");
        ipToRsu["10.0.0.26"] = getModuleByPath("RSUExampleScenario.rsu[3].appl");

        ipToRsu["10.0.0.54"] = getModuleByPath("RSUExampleScenario.rsu[4].appl");
        ipToRsu["10.0.0.30"] = getModuleByPath("RSUExampleScenario.rsu[5].appl");
        ipToRsu["10.0.0.34"] = getModuleByPath("RSUExampleScenario.rsu[6].appl");
        ipToRsu["10.0.0.58"] = getModuleByPath("RSUExampleScenario.rsu[7].appl");

        ipToRsu["10.0.0.38"] = getModuleByPath("RSUExampleScenario.rsu[8].appl");
        ipToRsu["10.0.0.18"] = getModuleByPath("RSUExampleScenario.rsu[9].appl");
        ipToRsu["10.0.0.14"] = getModuleByPath("RSUExampleScenario.rsu[10].appl");
        ipToRsu["10.0.0.22"] = getModuleByPath("RSUExampleScenario.rsu[11].appl");

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
                << " --simtime " << SIMTIME_DBL(start)
                << " --out-decisions " << outDecisions
//                << " --T " << inferInterval
                << " --out-inference "<< inference_outputs
                << " --out-inference-overhead "<< inference_overhead
                << " --out-clusters "<< clusters_output
                << " >> /tmp/python_output_infer.log 2>&1"; // This is the magic line


        auto wallAfterPy = std::chrono::steady_clock::now();
        double t_launch_ms = std::chrono::duration<double, std::milli>(wallAfterPy - wallStart).count();

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
            writeControllerOverhead(t_launch_ms, 0.0, 0);
            return;
        }

        applyDecisions(outDecisions, SIMTIME_DBL(start));

        std::ifstream fcluster(clusters_output);
        if (!fcluster.good()) {
                EV_WARN << "[Controller] Could not open clusters file: " << clusters_output << "\n";
                writeControllerOverhead(t_launch_ms, 0.0, 0);
                return;
        }
        applyClusters(clusters_output,SIMTIME_DBL(start));

        auto wallAfterApply = std::chrono::steady_clock::now();
        double t_apply_ms  = std::chrono::duration<double, std::milli>(wallAfterApply - wallAfterPy).count();
        writeControllerOverhead(t_launch_ms, t_apply_ms, 1);


    }
    void GnnModel::writeControllerOverhead(double t_launch_ms, double t_apply_ms, int hasDecision){
        // log
        std::ostringstream oss;
        oss << SIMTIME_DBL(simTime()) << "," << t_launch_ms << "," << t_apply_ms << "," << ctrl_msgs_sent<< "," << hasDecision;
        //        appendCsv(controller_overhead,
        //                  "simTime,t_launch_ms,t_apply_ms,ctrl_msgs_sent",
        //                  oss.str());

        struct stat st;
        bool writeHeader = (stat(controller_overhead.c_str(), &st) != 0 || st.st_size == 0);

        std::ofstream fr(controller_overhead, std::ios::app);
        if (writeHeader) fr << "simTime,t_launch_ms,t_apply_ms,ctrl_msgs_sent,hasDecision" << "\n";
        fr << oss.str() << "\n";
    }
    // in GnnModel.cc
    void GnnModel::applyClusters(const std::string& clustersFile, double simNow) {
        int window = (int)SIMTIME_DBL(inferInterval);
        const int lower   = ((int) simNow) - window;
        std::unordered_map<std::string, std::vector<std::pair<int, std::vector<int>>>> clustersByRSU;
        // Seen set to keep only the latest entry per (rsuIP, CH) while scanning backwards
        std::unordered_set<std::string> seen; // key = rsuIP + "|" + chStr


        std::ifstream f(clustersFile);

        std::vector<std::string> lines;
        std::string line;
        std::getline(f, line); // skip header
        while (std::getline(f, line)) lines.push_back(std::move(line));
        f.close();
        // 2) Walk backwards: most recent rows first
        for (auto it = lines.rbegin(); it != lines.rend(); ++it) {
            const std::string& row = *it;
            if (row.empty()) continue;
            std::istringstream iss(row);
            std::string simTimeStr, rsuIP, chStr, membersStr;

            if (!std::getline(iss, simTimeStr, ',')) continue;
            if (simTimeStr.empty()) continue;

            int simTime = std::stoi(simTimeStr);
            if (simTime < lower) break;
            if (!std::getline(iss, rsuIP, ',')) continue;
            if (!std::getline(iss, chStr, ',')) continue;
            if (!std::getline(iss, membersStr)) continue;
            // Deduplicate the latest occurrence for (RSU, CH)
            std::string key = rsuIP + "|" + chStr;
            if (seen.find(key) != seen.end()) continue;
            seen.insert(key);
            int clusterHead = std::stoi(chStr); ;

                            // Parse members (space-separated ints)
            std::vector<int> members;
            std::istringstream mss(membersStr);
            int v;
            while (mss >> v) {
                members.push_back(v);
            }
            clustersByRSU[rsuIP].push_back({clusterHead, std::move(members)});
        }
        for (const auto& [rsuIP, clusters] : clustersByRSU) {
            for (const auto& [ch, members] : clusters) {
                sendClusterInfo(rsuIP, ch, members);

                // takeActionForCluster(rsuIP, ch, members); // if you also act here
            }
        }


    }

    void GnnModel::applyDecisions(const std::string& path, double simNow) {
        //-------------------debug----------------------
//        std::ostringstream ossDebug;
//        std::ofstream fdebug('/home/nadia/controller_debug.csv', std::ios::app);

        std::ifstream f(path);
        static int decisionId = 0;

        std::vector<std::string> lines;
        std::string line;
        std::getline(f, line); // skip header
        while (std::getline(f, line)) lines.push_back(std::move(line));
        f.close();

        int Max_t = 0;
        for (auto it = lines.rbegin(); it != lines.rend(); ++it) {
            const std::string& row = *it;
            if (row.empty()) continue;
            std::istringstream iss(row);
            std::string simTime, decisionId, type, vehicleID, srcRSU, dstRSU;
            if (!std::getline(iss, simTime, ',')) continue;
            //            std::getline(iss, simTime, ',');
            if (simTime.empty()) continue;
            int simTime_check = std::stoi(simTime);

            if (simTime_check >= Max_t) Max_t = simTime_check;
            else break;
            std::getline(iss, decisionId, ',');
            std::getline(iss, type, ',');
            std::getline(iss, vehicleID, ',');
            std::getline(iss, srcRSU, ',');
            std::getline(iss, dstRSU, ',');
            if (type.compare("attach") == 0) {

                std::vector<int> members;
                std::istringstream mss(vehicleID);
                int v;
                while (mss >> v) {
                    members.push_back(v);
                }

//                lastAttachState[dstRSU] = {members};


                sendAttachBatch(members, dstRSU);
            }else if (type.compare("reroute") == 0 ){
                int vid = std::stoi(vehicleID);
//                sendReroute(vid, dstRSU, srcRSU, simTime);
                std::vector<int> members;
                members.push_back(vid);
                sendAttachBatch(members, dstRSU);
                sendDetachBatch(members, srcRSU, simTime);

            }else if (type.compare("detach") == 0){
                std::vector<int> members;
                std::istringstream mss(vehicleID);
                int v;
                while (mss >> v) {
                    members.push_back(v);
                }

                sendDetachBatch(members, dstRSU, simTime);
            }


        }


    }

        // helper to print vectors
    static std::string vecToStr(const std::vector<int>& v) {
        std::ostringstream oss; oss << "[";
        for (size_t i=0;i<v.size();++i) { if (i) oss<<","; oss<<v[i]; }
        oss << "]";
        return oss.str();
    }
    void GnnModel::sendClusterInfo(const std::string& rsuIP, int clusterHead, const std::vector<int>& members) {
        auto it = ipToRsu.find(rsuIP);
        if (it == ipToRsu.end()) {
            EV_WARN << "[Controller] Unknown RSU IP " << rsuIP << "\n";
            return;
        }

        // Create a controller command message
        auto *cmdMsg = new ControllerCommand("clusterInfo");
        cmdMsg->setCmd("cluster");  // New command type "cluster"
        cmdMsg->setTargetRSU(rsuIP.c_str());

        // Set cluster head
        cmdMsg->setClusterHead(clusterHead);

        // Set members (excluding CH if you want only "followers")
        cmdMsg->setVehicleIDsArraySize(members.size());
        for (size_t i = 0; i < members.size(); i++) {
            cmdMsg->setVehicleIDs(i, members[i]);
        }

        // Send directly to RSU
        sendDirect(cmdMsg, it->second, "controlIn");
        EV_INFO << "[Controller] Sent cluster info to RSU " << rsuIP
                << " head=" << clusterHead
                << " members=" << vecToStr(members) << "\n";

        ctrl_msgs_sent++;
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

//        for (size_t i = 0; i < vehicles.size(); i++) {
//            std::stringstream cmd;
//            cmd << "sh -c \""<< "echo attach rsu=" << targetRSU
//                << "echo member " << vehicles[i] << " >> /tmp/decision.log"
//                << "\"";
//            int rc = std::system(cmd.str().c_str());
//            (void)rc;
//        }
        sendDirect(cmdMsg, it->second, "controlIn");
        EV_INFO << "[Controller] Sent attach batch of " << vehicles.size()
                    << " vehicles to RSU " << targetRSU << "\n";
        ctrl_msgs_sent++;
    }
    void GnnModel::sendDetachBatch(const std::vector<int>& vehicles, const std::string& targetRSU, std::string& rrTime) {
        if (vehicles.empty()) return;
        auto it = ipToRsu.find(targetRSU);
        if (it == ipToRsu.end()) {
            EV_WARN << "[Controller] Unknown RSU IP " << targetRSU << "\n";
            return;
        }
        auto *cmdMsg = new ControllerCommand("detachBatch");
        cmdMsg->setCmd("detach");
        cmdMsg->setTargetRSU(targetRSU.c_str());
        cmdMsg->setVehicleIDsArraySize(vehicles.size());
        cmdMsg->setRerouteTime(rrTime.c_str());

        for (size_t i = 0; i < vehicles.size(); i++) {
                    cmdMsg->setVehicleIDs(i, vehicles[i]);
        }

        sendDirect(cmdMsg, it->second, "controlIn");
        EV_INFO << "[Controller] Sent detach batch of " << vehicles.size()
                            << " vehicles to RSU " << targetRSU << "\n";
        ctrl_msgs_sent++;
    }
    void GnnModel::sendReroute(int vehicleID, const std::string& targetIP,const std::string& srcRSU, std::string& rrTime) {
        auto it = ipToRsu.find(targetIP);
        if (it == ipToRsu.end()) {
            EV_WARN << "[Controller] Unknown RSU IP " << targetIP << "\n";
            return;
        }

        auto *cmdMsg = new ControllerCommand("reroute");
        cmdMsg->setCmd("reroute");
        cmdMsg->setTargetRSU(targetIP.c_str());
        cmdMsg->setRerouteTime(rrTime.c_str());

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
