//
// Copyright (C) 2016 David Eckhoff <david.eckhoff@fau.de>
//
// Documentation for these modules is at http://veins.car2x.org/
//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "veins/modules/application/traci/TraCIDemoRSU11p.h"

#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"
#include "veins/modules/obstacle/ObstacleControl.h"

// Standard INET 3.8 headers
#include "inet/transportlayer/contract/udp/UDPControlInfo_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/linklayer/common/MACAddress.h"
#include "inet/controller_messages/ControllerCommand_m.h"
#include "inet/networklayer/contract/IInterfaceTable.h"
#include "inet/networklayer/ipv4/IPv4InterfaceData.h"
#include "inet/common/ModuleAccess.h"

#include "veins/modules/messages/BaseFrame1609_4_m.h"
#include "veins/modules/messages/DemoSafetyMessage_m.h"
#include "veins/modules/messages/ClusterCommand_m.h"
#include "veins/base/phyLayer/PhyToMacControlInfo.h"


#include "veins/base/modules/BaseMobility.h"
#include "veins/base/utils/FindModule.h"
#include "veins/modules/application/traci/TraCIDemo11p.h"


#include <omnetpp.h>

// Standard library includes
#include <string>
#include <vector>
#include <stdexcept> // For std::runtime_error used in MACAddress constructor
#include <sys/stat.h>
#include <fstream>

// ** FIX: Placing using namespace statements after all includes **
using namespace veins;
using namespace inet;

Define_Module(TraCIDemoRSU11p);

//const int OPENFLOW_UDP_PORT = 6000;
const int RSU_UDP_PORT = 5000;

void TraCIDemoRSU11p::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);
    EV_INFO << "***************************rsu********************++++++++++++++++"<< endl;
    if (stage == 0) {
        EV_INFO << "***************************RSU Initialize********************++++++++++++++++"<< endl;
        udpSocket.setOutputGate(gate("socketOut"));
        udpSocket.bind(L3Address(), RSU_UDP_PORT);
        EV_INFO << "RSU application bound to UDP port " << RSU_UDP_PORT << endl;
        detachTable.clear();  // empty at start
        enforceTable = false;
        RSU_stats_DataFileName = par("RSU_stats_DataFileName").stdstringValue();
        rxCount = txCount = dropCount = 0;

        statsTimer = new cMessage("statsTimer");
        scheduleAt(simTime() + statsInterval, statsTimer);
    }
}


void TraCIDemoRSU11p::onWSM(BaseFrame1609_4* wsm)
{
    EV_INFO << "***************************onwsm********************++++++++++++++++"<< endl;


    // Decapsulate to get the application message
    TraCIDemo11pMessage* vehicleMessage = dynamic_cast<TraCIDemo11pMessage*>(wsm->getEncapsulatedPacket());
    if (!vehicleMessage) {
        delete wsm;
        return;
    }

    // --- Get Sender's Position ---
    // The position is attached to the frame as control info by the physical layer
    Coord senderPos = vehicleMessage->getNodeMobilityCoord();
    // --- Create a New Message for the Controller ---
    // We create a new message to add the position data.
    TraCIDemo11pMessage* msgForController = vehicleMessage->dup();

    // Get the controller's address and port
    L3Address controllerIp = L3AddressResolver().resolve("open_flow_controller1");
    int controllerPort = par("controllerPort");

    EV_INFO << "RSU received message from vehicle " << msgForController->getSenderAddress()
                                                                        << " at position (" << senderPos.x << "," << senderPos.y
                                                                        << "), forwarding to controller." << endl;

    // Send the new message. The socket takes ownership of the pointer.
    udpSocket.sendTo(msgForController, controllerIp, controllerPort);

    // We are now done with the original incoming frame.
    delete wsm;



}

//void TraCIDemoRSU11p::onWSA(DemoServiceAdvertisment* wsa) {
//    EV_INFO << "RSU " << getParentModule()->getFullName() << " received WSA: " << wsa->getName() << endl;
//    DemoBaseApplLayer::onWSA(wsa);
//}
// This function handles all incoming messages for the module
//inline void appendCsv(const std::string& path, const std::string& line) {
//    std::ofstream f(path, std::ios::app);
//    f << line << "\n";
//}

//inline void TraCIDemoRSU11p::appendRSUCsv(const std::string& path,
//                      const std::string& header,
//                      const std::string& line) {
//    struct stat st;
//    bool writeHeader = (stat(path.c_str(), &st) != 0 || st.st_size == 0);
//
//    std::ofstream f(path, std::ios::app);
//    if (writeHeader) f << header << "\n";
//    f << line << "\n";
//}
void TraCIDemoRSU11p::setIP(){
    if (myIP == "0.0.0.0") {

        inet::IPv4Address rsuIP;

        inet::IInterfaceTable *itable = check_and_cast<inet::IInterfaceTable*>(
                getParentModule()->getSubmodule("interfaceTable")
        );

        // 2. Iterate through all network interfaces to find the one with an IP.
        for (int i = 0; i < itable->getNumInterfaces(); i++) {
            inet::InterfaceEntry *ie = itable->getInterface(i);
            // Look for the first valid, non-loopback IPv4 address
            if (!ie->isLoopback() && !ie->getIPv4Address().isUnspecified()) {
                rsuIP = ie->getIPv4Address();
                myIP = rsuIP.str();
                EV_INFO << "My RSU IP Address is: " << rsuIP.str() << std::endl;
                break; // Found the address, so we can stop searching
            }
        }

        // 3. Now you can use the IP address.
        if (!rsuIP.isUnspecified()) {
            EV << "My RSU IP Address is: " << rsuIP.str() << std::endl;
            myIP = rsuIP.str();
        }else{
            EV_ERROR<< "rsu ip didn't found!"<<endl;
        }
    }
}
cModule* TraCIDemoRSU11p::findVehicleModule(int vehicleId) {
    //    std::ostringstream ossDebug; //debug
    //    std::ofstream fDebug(RSU_stats_DataFileName, std::ios::app); //debug

    // The top-level network module
    cModule* network = getSimulation()->getSystemModule();

    // Assumes vehicles are in a submodule array called "node"
    cModule* vehiclesContainer = network->getSubmodule("node");
    if (!vehiclesContainer) {
        EV_WARN << "[RSU] No 'node' submodule found in network\n";
        //        ossDebug << "[RSU] No 'node' submodule found in network\n";
        //        fDebug << ossDebug.str() << "\n"; //debug
        return nullptr;
    }

    // Look through all vehicles to match the ID
    for (cModule::SubmoduleIterator it(vehiclesContainer); !it.end(); ++it) {
        cModule* veh = *it;
        // vehicle IDs are often set in the application layer (TraCIDemo11p)
        cModule* appl = veh->getSubmodule("appl");
        if (!appl) continue;

        auto* appLayer = dynamic_cast<TraCIDemo11p*>(appl);
        //        if (appLayer && appLayer->getVehicleId() == vehicleId) {
        //            ossDebug << "[RSU] 'node' submodule found in network++++++++++\n";
        //            fDebug << ossDebug.str() << "\n"; //debug
        //            return appl; // found the right vehicle application module
        //        }
    }

    EV_WARN << "[RSU] Could not find vehicle module for ID=" << vehicleId << "\n";
    //    ossDebug << "[RSU] Could not find vehicle module for ID=" << vehicleId << "\n";
    //            fDebug << ossDebug.str() << "\n"; //debug
    return nullptr;
}

void TraCIDemoRSU11p::sendClusterHead(int chId, const std::vector<int>& members) {
    auto *msg = new ClusterCommand("setCH");
    msg->setCmd("setCH");
    msg->setClusterHead(chId);
    msg->setClusterMembersArraySize(members.size());
    for (size_t i = 0; i < members.size(); i++) {
        msg->setClusterMembers(i, members[i]);
    }


    populateWSM(msg);
    sendDown(msg);
    EV_INFO << "[RSU] Assigned vehicle " << chId << " as Cluster Head for "
            << members.size() << " members\n";
}
void TraCIDemoRSU11p::sendClusterMembers(int chId, const std::vector<int>& members) {
    for (int v : members) {
        if (v == chId) continue; // skip the CH itself
        auto *msg = new ClusterCommand("setMember");
        msg->setCmd("setMember");
        msg->setClusterHead(chId);
        msg->setClusterMembersArraySize(0); // not needed for members

        populateWSM(msg);
        sendDown(msg);
        EV_INFO << "[RSU] Informed vehicle " << v
                << " that its Cluster Head is " << chId << "\n";
    }
}

void TraCIDemoRSU11p::writeNetworkMetricSVM(){

    uint64_t txPktsWinSum = 0;

    std::ofstream f(RSU_stats_DataFileName, std::ios::app);

    for (auto& kv : vehTxCumLatest) {
        int vid = kv.first;
        uint64_t latest = kv.second;
        uint64_t base   = vehTxCumBaseline[vid];  // default 0 on first window
        uint64_t delta  = (latest >= base) ? (latest - base) : latest;
        txPktsWinSum += delta;

    }

    uint64_t rxPktsInWindow = rxPktsWin - lastTotalRxPkts;
    // --- metrics ---
    double tx_pps        = txPktsWinSum / SIMTIME_DBL(statsInterval);
    double rx_pps        = rxPktsInWindow     / SIMTIME_DBL(statsInterval);
    double throughput_bps= (8.0 * rxBytesWin) / SIMTIME_DBL(statsInterval);
    double plr           = (txPktsWinSum > 0) ? (1.0 - (double)rxPktsInWindow / (double)txPktsWinSum) : 0.0;

    std::ostringstream ln;
    ln.setf(std::ios::fixed); ln.precision(6);
    ln << SIMTIME_DBL(simTime()) << ","
            << myIP << ','
            << tx_pps << ','
            << rx_pps << ','
            << throughput_bps << ','
            << plr << ','
            << dropCount;;

    struct stat st;
    bool writeHeader = (stat(RSU_stats_DataFileName.c_str(), &st) != 0 || st.st_size == 0);

    //    std::ofstream f(metricsPath, std::ios::app);
    if (writeHeader) f << "time,myIP,tx_pps,rx_pps,throughput_bps,plr,dropCount" << "\n";
    f << ln.str() << "\n";

    lastRxpps = rx_pps;
    lastTxpps = tx_pps;
    lastThroughput = throughput_bps;
    lastPlr = plr;
    lastDropCount = (double)dropCount;
    lastDensity = activeVehicles.size();
    activeVehicles.clear();

    vehTxCumBaseline = vehTxCumLatest; // new baseline

    lastTotalRxPkts = rxPktsWin;
    //    rxPktsWin = rxBytesWin = 0;
    rxBytesWin = 0;
    //    lastTotalTxPkts = txPktsWinSum;

}
void TraCIDemoRSU11p::handleMessage(cMessage* msg)
{
    EV_INFO << "------------------------------handlemessage---------"<<endl;
    //    std::ostringstream ossDebug; //debug
    //    std::ofstream fDebug(RSU_stats_DataFileName, std::ios::app);//debug



    if (msg == statsTimer) {

        simtime_t now = simTime();  // current OMNeT++ simulation time
        int activeCount = 0;

        //        // Iterate through all detach entries
        //        for (const auto& kv : detachTable) {
        //            int vehId = kv.first;
        //            const AttachState& st = kv.second;
        //
        //            if (st.expireTime > now) {
        //                // still active
        //                activeCount++;
        //                EV_INFO << "[Debug] Vehicle " << vehId
        //                        << " blocked until " << st.expireTime << "\n";
        //            }
        //        }



        setIP();
        writeNetworkMetricSVM();

// my approach--------------------------------------
//        double rxpps = rxCount / SIMTIME_DBL(statsInterval);
//        double txpps = txCount / SIMTIME_DBL(statsInterval);
//
//        std::ostringstream oss;
//        oss << SIMTIME_DBL(simTime()) << "," << myIP << "," << rxpps << "," << txpps << "," << dropCount;
//        //                oss<< now << "," << detachTable.size()<<"," << myIP << "," << activeCount << "," << (detachTable.size() - activeCount);
//
//
//        struct stat st;
//        bool writeHeader = (stat(RSU_stats_DataFileName.c_str(), &st) != 0 || st.st_size == 0);
//
//        std::ofstream f(RSU_stats_DataFileName, std::ios::app);
//        if (writeHeader) f << "simTime,myIP,rxpps, txpps, dropCount" << "\n";
//        f << oss.str() << "\n";
//

// my approach--------------------------------------

        rxCount = txCount = dropCount = 0;
        scheduleAt(simTime() + statsInterval, statsTimer);
        return;
    }
    if (auto *cmd = dynamic_cast<ControllerCommand*>(msg)) {
        EV_INFO<< "RSU: Received decision from Controller. Decision is: "<<cmd->getCmd() <<endl;
        std::string cmdName = cmd->getCmd();

        if (cmdName.compare("attach") == 0) {
            // Batch of vehicles to attach
            int n = cmd->getVehicleIDsArraySize();
            EV_INFO << "[RSU] Attach request for " << n
                    << " vehicles at RSU " << getFullPath() << "\n";

            for (int i = 0; i < n; i++) {
                int vid = cmd->getVehicleIDs(i);
                EV_INFO << "  -> Attaching vehicle " << vid << "\n";
//                handleAttachVehicle(vid); //+++++++++++
            }
            //
            enforceTable = true; //++++++++++++++++
        }else if (cmdName.compare("detach") == 0) {
            // Batch of vehicles to attach
            int n = cmd->getVehicleIDsArraySize();
            std::string rrtime = cmd->getRerouteTime();

            EV_INFO << "[RSU] Detach request for " << n
                    << " vehicles at RSU " << getFullPath() << "\n";

            for (int i = 0; i < n; i++) {
                int vid = cmd->getVehicleIDs(i);
                EV_INFO << "  -> Detaching vehicle " << vid << "\n";
                //                handleDetachVehicle(vid, rrtime); //+++++++++++
            }

            enforceTable = true; //++++++++++++++++
        }
        else if (cmdName.compare("reroute") == 0) {
            // Single reroute (one vehicle)
            int n = cmd->getVehicleIDsArraySize();
            if (n > 0) {
                int vid = cmd->getVehicleIDs(0);
                std::string tgt = cmd->getTargetRSU();
                std::string rrtime = cmd->getRerouteTime();

                EV_INFO << "[RSU] Reroute request for vehicle " << vid
                        << " to RSU " << tgt << "\n";
                //                handleDetachVehicle(vid, rrtime);
            }
            enforceTable = true;
        } else if (cmdName.compare("cluster") == 0){
            int ch = cmd->getClusterHead();
            int n = cmd->getVehicleIDsArraySize();

            std::vector<int> members;
            members.reserve(n);

            EV_INFO << "[RSU] Cluster assignment at RSU " << getFullPath() << "\n";
            EV_INFO << "  -> Cluster Head: " << ch << "\n";
            EV_INFO << "  -> Cluster Members (" << n << "): ";
            for (int i = 0; i < n; i++) {
                int vid = cmd->getVehicleIDs(i);
                members.push_back(vid);

            }
            sendClusterHead(ch, members); //++++++++

        }
        delete cmd;
        return;
    }


    //        // Check if it's a wireless message from a vehicle
    if (msg->getArrivalGate() == gate("lowerLayerIn")) {
        EV_ERROR << "--> RSU DEBUG: Received message from wireless NIC. Class Name is: " << msg->getClassName() << endl;


        if(TraCIDemo11pMessage* v2v_msg = dynamic_cast<TraCIDemo11pMessage*>(msg)){
            if (v2v_msg->getSenderType() == VEHICLE) {
                rxCount++;
                auto it = detachTable.find(v2v_msg->getSenderAddress());
                if (it != detachTable.end() ){
                    if (simTime() < it->second.expireTime){
                        EV_WARN << "[RSU] Dropping packet from vehicle " << v2v_msg->getSenderAddress()
                                                                                                    << " (detached, TTL active)\n";
                        dropCount++;
                        delete v2v_msg;
                        return;
                    }else if (simTime() > it->second.expireTime){
                        detachTable.erase(v2v_msg->getSenderAddress());  // cleanup if expired
                        EV_INFO << "[RSU] Processing packet from vehicle " << v2v_msg->getSenderAddress() << "\n";
                    }
                }

                //                    if (enforceTable){
                //                        if (!attachedVehicles.count(v2v_msg->getSenderAddress())){
                //                            EV_ERROR<< "RSU doesn't have the vehicle: "<< v2v_msg->getSenderAddress()<< " in its cluster, packet dropped."<<endl;
                //                            delete v2v_msg;
                //                            return;
                //                        }
                //                    }

                EV_INFO << "RSU received TraCIDemo11pMessage from vehicle, forwarding to controller." << endl;

                Coord senderPos = v2v_msg->getNodeMobilityCoord();

                L3Address controllerIp = L3AddressResolver().resolve("open_flow_controller1");
                int controllerPort = par("controllerPort");

                cPacket* payload = v2v_msg->dup();
                EV_INFO << "RSU received message from vehicle " << v2v_msg->getSenderAddress()
                                                                                            << " at position (" << senderPos.x << "," << senderPos.y
                                                                                            << "), forwarding to controller." << endl;
                // when forwarding to another RSU/controller:
                txCount++;
                udpSocket.sendTo(payload, controllerIp, controllerPort);

                // We are now done with the original incoming wireless message.
                delete v2v_msg;
            }
        }
        else if (DemoSafetyMessage* bsm = dynamic_cast<DemoSafetyMessage*>(msg)){
            if (bsm->getSenderType() == VEHICLE) {


                rxCount++;

                EV_INFO << "RSU received a DemoSafetyMessage (Beacon) from vehicle, forwarding to controller." << endl;
                EV_ERROR<< "Debug:Vehicle: The time stamp of original packet is: "<< bsm->getTimestamp()<<endl;
                // The BSM is a BaseFrame1609_4, which holds the sender's address and position.
                BaseFrame1609_4* frame = check_and_cast<BaseFrame1609_4*>(bsm);

                TraCIDemo11pMessage* msgForController = new TraCIDemo11pMessage();
                LAddress::L2Type senderAddress;
                senderAddress = bsm->getOriginalSenderId();
                bool hasCluster = bsm->getHasCluster();
                bool sendFromCH = bsm->getSendFromCH();
                //                -----------------cluster----------------
                //                if (hasCluster){
                //
                //                    if (!sendFromCH){
                //                        dropCount++;
                //                        delete bsm;
                //                        return;
                //                    }
                //                }
                //                -----------------cluster----------------

                //                --------smv based-------
                vehTxCumLatest[senderAddress] = bsm->getTxPktsCumulative();
                rxPktsWin++;
                rxBytesWin += (bsm->getByteLength());
                activeVehicles.insert(senderAddress);

//                msgForController->setSVM_density(lastDensity);
//                msgForController->setSVM_dropCount(lastDropCount);
//                msgForController->setSVM_plr(lastPlr);
//                msgForController->setSVM_rx_pps(lastRxpps);
//                msgForController->setSVM_throughput_bps(lastThroughput);
//                msgForController->setSVM_tx_pps(lastTxpps);

                //                --------smv based-------

                //                //-----------------------ADR-----------------------------
                //                auto it = detachTable.find(senderAddress);
                //                if (it != detachTable.end() ){
                //                    if ( simTime() < it->second.expireTime){
                //                        EV_WARN << "[RSU] Dropping packet from vehicle " << senderAddress
                //                                << " (detached, TTL active)\n";
                //                        dropCount++;
                //                        delete bsm;
                //                        return;
                //                    }else if (simTime() > it->second.expireTime){
                //                        detachTable.erase(senderAddress);  // cleanup if expired
                //                        EV_INFO << "[RSU] Processing packet from vehicle " << senderAddress << "\n";
                //                    }
                //                }
                //                --------------------------ADR--------------------------------------------
                //                //
                //                    if (enforceTable){
                //                        if (!attachedVehicles.count(senderAddress)){
                //                            EV_ERROR<< "RSU doesn't have the vehicle: "<< senderAddress<< " in its cluster, packet dropped."<<endl;
                //                            delete bsm;
                //                            return;
                //                        }
                //                    }
                //                    //----------------------------------------------------
                //                }
                //                else {
                //                    // If control info is missing, we can't process the beacon
                //                    EV_ERROR << "RSU received beacon without PhyToMacControlInfo. Cannot process." << endl;
                //                    delete bsm;
                //                    //                           return; // Stop processing this message
                //                }
                // --- FINAL FIX ---
                // Get the sender's address and position from the parent frame object.
                msgForController->setSenderAddress(senderAddress);
                msgForController->setNodeMobilityCoord(bsm->getSenderPos());
                msgForController->setDemoData("Beacon Forwarded by RSU");
                msgForController->setTimestamp(bsm->getTimestamp());
                msgForController->setByteLength(bsm->getByteLength());
                msgForController->setSpeed(bsm->getSpeed());
                msgForController->setHasCluster(hasCluster);
                msgForController->setSendFromCH(sendFromCH);
                msgForController->setTxPktsCumulative(bsm->getTxPktsCumulative());

                msgForController->setDirection(bsm->getDirection());

                L3Address controllerIp = L3AddressResolver().resolve("open_flow_controller1");
                int controllerPort = par("controllerPort");

                EV_ERROR<< "Debug:Vehicle: The time stamp of sending packet is: "<< msgForController->getTimestamp()<<endl;
                // when forwarding to another RSU/controller:
                txCount++;
                udpSocket.sendTo(msgForController, controllerIp, controllerPort);

                delete bsm;
            }
        }
    }
    // Else, check if it's a UDP packet from the switch/controller
    else if (msg->getArrivalGate() == gate("socketIn")) {

        if (cPacket* udpPacket = dynamic_cast<cPacket*>(msg)){
            EV_INFO << "RSU received UDP packet from switch, broadcasting to vehicles." << endl;

            cPacket* appMsgPayload = udpPacket->decapsulate();

            BaseFrame1609_4* frame = new BaseFrame1609_4();
            frame->setChannelNumber(172);
            frame->encapsulate(appMsgPayload);


            sendDown(frame);
            delete udpPacket;
            return;
        }
    }
    else DemoBaseApplLayer::handleMessage(msg);

}
void TraCIDemoRSU11p::handleAttachVehicle(int vehicleID) {
    auto it = detachTable.find(vehicleID);
    if (it != detachTable.end()) {
        detachTable.erase(it);
        EV_INFO << "[RSU] Vehicle " << vehicleID
                << " re-attached, block entry removed\n";
    } else {
        EV_INFO << "[RSU] Attach command for vehicle " << vehicleID
                << " ignored (no block entry)\n";
    }
}

void TraCIDemoRSU11p::handleDetachVehicle(int vehicleID, std::string& rrtime) {
    simtime_t t = simtime_t(std::stod(rrtime.c_str()));

    detachTable[vehicleID] = { t + detachTTL };
    simtime_t temp = detachTable[vehicleID].expireTime;
    EV_INFO << "[RSU] Vehicle " << vehicleID
            << " detached until " << temp << "\n";

    attachedVehicles.erase(vehicleID);
    EV_INFO << "[RSU] Vehicle " << vehicleID << " detached\n";
}
