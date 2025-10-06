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

#pragma once

// Forward declare custom message type for the new method
#ifndef VEINS_TRACIDEMORSUNOP11P_H_
#define VEINS_TRACIDEMORSUNOP11P_H_

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "inet/transportlayer/contract/udp/UDPSocket.h"

#include "veins/modules/messages/BaseFrame1609_4_m.h"        // For BaseFrame1609_4
#include "veins/modules/messages/DemoServiceAdvertisement_m.h" // For DemoServiceAdvertisment
#include "veins/modules/messages/DemoSafetyMessage_m.h"
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"
#include <sys/stat.h>
#include <fstream>
#include <unordered_map>   // <-- required for std::unordered_map


namespace veins {

/**
 * @brief Application layer for an RSU to demonstrate TraCI (SUMO) integration
 * and OpenFlow interaction via an INET network stack.
 */
class VEINS_API TraCIDemoRSU11p : public DemoBaseApplLayer {
protected:
    struct AttachState {
        simtime_t expireTime;  // after this, RSU accepts vehicle again
    };

    std::map<int, AttachState> detachTable;  // vehicles temporarily blocked
    simtime_t detachTTL = 10; // seconds

    inet::UDPSocket udpSocket;
    std::set<int> attachedVehicles;
    bool enforceTable = false;

    long rxCount = 0, txCount = 0, dropCount = 0;
    cMessage *statsTimer;
    simtime_t statsInterval = 1.0;
    std::string RSU_stats_DataFileName;
    std::string myIP = "0.0.0.0";


    // ---- svm based ----
    uint64_t rxPktsWin = 0;
    uint64_t rxBytesWin = 0;

    uint64_t totalTxPkts = 0;
    uint64_t totalRxPkts = 0;
    uint64_t lastTotalTxPkts = 0;
    uint64_t lastTotalRxPkts = 0;


        // ---- vehicle TX cumulative counters from beacons ----
    std::unordered_map<int, uint64_t> vehTxCumLatest;   // vehId -> cumulative TX pkts
    std::unordered_map<int, uint64_t> vehTxCumBaseline; // snapshot at last window edge

    double lastRxpps=0;
    double lastTxpps=0;
    double lastThroughput=0;
    double lastPlr=0;
    double lastDropCount=0;
    double lastDensity = 0;
    std::unordered_set<long> activeVehicles; // track unique vids this second
//    ------------------svm based---------------


protected:
    void onWSM(BaseFrame1609_4* wsm) override;
//    void onWSA(DemoServiceAdvertisment* wsa) override;
//    void handleMessageFromOpenFlowSwitch(openflow::FromOpenFlowSwitchMessage* msg);
    void handleMessage(cMessage* msg) override;
    void handleAttachVehicle(int vehicleID);
    void handleDetachVehicle(int vehicleID,  std::string& rrtime);
    void sendClusterMembers(int chId, const std::vector<int>& members);
    void sendClusterHead(int chId, const std::vector<int>& members);
    cModule* findVehicleModule(int vehicleId);
    void setIP();

    void writeNetworkMetricSVM(); //svm
//    inline void appendRSUCsv(const std::string& path,
//                          const std::string& header,
//                          const std::string& line) ;
//    void handleBSM(DemoSafetyMessage* bsm);
//    void handleHostStateNotification(HostState::States state);
//    void handleSelfMessage(cMessage* msg);

public:
    virtual void initialize(int stage) override;
};

} // namespace veins

#endif /* VEINS_TRACIDEMORSUNOP11P_H_ */
