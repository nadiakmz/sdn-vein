//
// Copyright (C) 2006-2011 Christoph Sommer <christoph.sommer@uibk.ac.at>
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

#include "veins/modules/application/traci/TraCIDemo11p.h"

#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"
#include "veins/modules/messages/ClusterCommand_m.h"
#include <string>
#include <fstream>

using namespace veins;


Define_Module(veins::TraCIDemo11p);

void TraCIDemo11p::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        sentMessage = false;
        lastDroveAt = simTime();
        currentSubscribedServiceId = -1;
        packetsSent = 0;
        clusterInterval = par("clusterInterval");

        clusterTimer = new cMessage("clusterTimer");


        // If beaconing is enabled in the .ini file, start our custom beacon timer
        if (par("sendBeacons").boolValue()) {
                    beaconTimer = new cMessage("beaconTimer");
//                    scheduleAt(simTime() + par("beaconInterval").doubleValue(), beaconTimer);
        }
    }
}

//this ,method is related to clustering section. comment it if you don't want it
void TraCIDemo11p::handleLowerMsg(cMessage* msg){
//    std::ostringstream ossDebug; //debug
//    std::ofstream fDebug("/home/nadia/temp.csv", std::ios::app);//debug

    if (auto *cmsg = dynamic_cast<ClusterCommand*>(msg)) {
        std::string cmdName = cmsg->getCmd();
        int ch = cmsg->getClusterHead();
        int id_temp = static_cast<int>(myId);
        if (id_temp == ch){
            amCH = true;
            amMember = false;
            clusterMembers.clear();
            for (int i = 0; i < cmsg->getClusterMembersArraySize(); i++) {
                int vid = cmsg->getClusterMembers(i);
                if (vid != myId) { // avoid self in member list
                    clusterMembers.push_back(vid);
                }
            }
            if (clusterTimer->isScheduled()) {
                cancelEvent(clusterTimer);
            }
            scheduleAt(simTime() + clusterInterval, clusterTimer);
//            ossDebug << "The vehicle is CH, ID:: "<< myId;
//            fDebug << ossDebug.str() << "\n";
        }else{
            for (int i = 0; i < cmsg->getClusterMembersArraySize(); i++) {
                if (cmsg->getClusterMembers(i) == myId) {
                    amMember = true;
                    amCH = false;
                    CHId = ch;
                    clusterMembers.clear();
//                    ossDebug << "vehicle id is: "<< myId << ", member";
//                    fDebug << ossDebug.str() << "\n";
                    if (clusterTimer->isScheduled()) {
                            cancelEvent(clusterTimer);
                        }
                    scheduleAt(simTime() + clusterInterval, clusterTimer);
                    break;
                 }
            }

        }
    }else{
        if (amCH){
            if (DemoSafetyMessage* bsm = dynamic_cast<DemoSafetyMessage*>(msg)){
                if (bsm->getSenderType() == VEHICLE) {
                    LAddress::L2Type senderAddress;
                    senderAddress = bsm->getOriginalSenderId();
                    if (std::find(clusterMembers.begin(), clusterMembers.end(), senderAddress) != clusterMembers.end()) {
                        EV_INFO << "[CH " << myId << "] Got beacon from member " << senderAddress
                                    << ". Rebroadcasting..." << endl;
                                        // Re-broadcast packet
//                        packetsSent++;
//                        bsm->setTxPktsCumulative(packetsSent);
                        bsm->setSendFromCH(true);
                        auto* copy = bsm->dup();
                        sendDown(copy);
                    }

                }
            }
        }

    }
}
void TraCIDemo11p::onWSA(DemoServiceAdvertisment* wsa)
{
    if (currentSubscribedServiceId == -1) {
        mac->changeServiceChannel(static_cast<Channel>(wsa->getTargetChannel()));
        currentSubscribedServiceId = wsa->getPsid();
        if (currentOfferedServiceId != wsa->getPsid()) {
            stopService();
            startService(static_cast<Channel>(wsa->getTargetChannel()), wsa->getPsid(), "Mirrored Traffic Service");
        }
    }
}

void TraCIDemo11p::onWSM(BaseFrame1609_4* frame)
{
    EV_ERROR<< "DEBUG:VEHICLE: onWSM: the packet is: "<< frame->getClassName()<<endl;
    TraCIDemo11pMessage* wsm = check_and_cast<TraCIDemo11pMessage*>(frame);
    if (wsm->getHopCount()<3){

        EV_ERROR<< "DEBUG:VEHICLE: HOP COUNT IS:" << wsm->getHopCount()<<endl;
        findHost()->getDisplayString().setTagArg("i", 1, "green");

        if (mobility->getRoadId()[0] != ':') traciVehicle->changeRoute(wsm->getDemoData(), 9999);
        if (!sentMessage) {
            sentMessage = true;
            // repeat the received traffic update once in 2 seconds plus some random delay
            wsm->setSenderAddress(myId);
            wsm->setSerial(3);
            wsm->setHopCount(wsm->getHopCount() + 1);
            wsm->setSpeed(mobility->getSpeed());

            Coord headingVector = mobility->getCurrentDirection();
            double angleInRadians = atan2(headingVector.y, headingVector.x);
            wsm->setDirection(angleInRadians);

                    // --- ADD THIS LINE ---
                    // Before forwarding the message, update it with this vehicle's current position.


            wsm->setNodeMobilityCoord(mobility->getPositionAt(simTime()));


            scheduleAt(simTime() + 2 + uniform(0.01, 0.2), wsm->dup());
        }
    }
    delete wsm;
}


void TraCIDemo11p::handleSelfMsg(cMessage* msg)
{
    EV_ERROR<< "HandleSelfMsg, the packet is: "<< msg->getClassName()<<endl;
    if (msg == clusterTimer){
        amMember = false;
        amCH = false;
        CHId = -1;
        clusterMembers.clear();
        cancelEvent(clusterTimer);
    }
    else if (TraCIDemo11pMessage* wsm = dynamic_cast<TraCIDemo11pMessage*>(msg)) {
        // send this message on the service channel until the counter is 3 or higher.
        // this code only runs when channel switching is enabled
        wsm->setHopCount(wsm->getHopCount() + 1);
        wsm->setSenderType(VEHICLE);
        sendDown(wsm->dup());
        wsm->setSerial(wsm->getSerial() + 1);
        if (wsm->getSerial() >= 3) {
            // stop service advertisements
            stopService();
            delete (wsm);
        }
        else {
            scheduleAt(simTime() + 1, wsm);
        }
    }
    else if (msg->getKind()== SEND_BEACON_EVT){
        DemoSafetyMessage* bsm = new DemoSafetyMessage();
        packetsSent++;
        int pktSize = intuniform(100, 300);

//        bsm->setBitLength(headerLength);
        bsm->setByteLength(pktSize);

        bsm->setHopCount(0);
        bsm->setTimestamp(simTime());

        Coord headingVector = mobility->getCurrentDirection();
        double angleInRadians = atan2(headingVector.y, headingVector.x);
        bsm->setDirection(angleInRadians);
        bsm->setSpeed(mobility->getSpeed());

        bsm->setSenderPos(curPosition);
        bsm->setSenderSpeed(curSpeed);
        bsm->setPsid(-1);
        bsm->setChannelNumber(static_cast<int>(Channel::cch));
//        bsm->addBitLength(beaconLengthBits);
        bsm->setUserPriority(beaconUserPriority);
        bsm->setSenderType(VEHICLE);
        bsm->setOriginalSenderId(myId);
        bsm->setTxPktsCumulative(packetsSent);

        bsm->setHasCluster(false);
        bsm->setSendFromCH(false);
        if (amCH) bsm->setSendFromCH(true);
        if (amCH || amMember) bsm->setHasCluster(true);


        sendDown(bsm);

        scheduleAt(simTime() + beaconInterval, sendBeaconEvt);
    }
//    else if (msg == beaconTimer) { //beacon test
//        TraCIDemo11pMessage* bsm = new TraCIDemo11pMessage();
//        populateWSM(bsm); // Fill the beacon with data
//        sendDown(bsm);    // Send the beacon
//        scheduleAt(simTime() + par("beaconInterval").doubleValue(), beaconTimer); // Reschedule
//        return;
//    }
    else {

        DemoBaseApplLayer::handleSelfMsg(msg);
    }
}
void TraCIDemo11p::handlePositionUpdate(cObject* obj)
{


    DemoBaseApplLayer::handlePositionUpdate(obj);

    const static std::vector<Coord> rsuPositions = {
            {2246.59, 2038.29, 4.0},
            {2280.59, 860.29,  1.0},
            {1755.54, 1241.46, 1.0}
        };
    const double detectionRange = 300.0; // 300 meters
    Coord vehiclePos = mobility->getPositionAt(simTime());
    for (size_t i = 0; i < rsuPositions.size(); ++i) {
            double distance = vehiclePos.distance(rsuPositions[i]);

            // 4. If distance is less than the range, print a message
            if (distance < detectionRange) {
                EV_ERROR << "--> VEHICLE DEBUG: Vehicle " << myId
                         << " is now " << distance << "m away from RSU[" << i << "]" << endl;
            }
        }

    // stopped for for at least 10s?
    if (mobility->getSpeed() < 1) {
        EV_ERROR<< "HandlePositionUpdate, the speed is less than 1" <<endl;
        if (simTime() - lastDroveAt >= 10 && sentMessage == false) {

            findHost()->getDisplayString().setTagArg("i", 1, "red");
            sentMessage = true;

            TraCIDemo11pMessage* wsm = new TraCIDemo11pMessage();

//            populateWSM(wsm);
//            wsm->setNodeMobilityCoord(mobility->getPositionAt(simTime())); // Set the position
//            wsm->setDemoData(mobility->getRoadId().c_str());

            // host is standing still due to crash
//            if (dataOnSch) {
//                startService(Channel::sch2, 42, "Traffic Information Service");
//                // started service and server advertising, schedule message to self to send later
//                scheduleAt(computeAsynchronousSendingTime(1, ChannelType::service), wsm);
//            }
//            else {
//                // send right away on CCH, because channel switching is disabled
//                sendDown(wsm);
//                packetsSent++;
//            }
        }
    }
    else {
        lastDroveAt = simTime();
    }
}
void TraCIDemo11p::finish()
{
    recordScalar("packetsSent", packetsSent);
    cancelAndDelete(clusterTimer);
}
//
////beacon
//void TraCIDemo11p::populateWSM(TraCIDemo11pMessage* wsm) {
//    DemoBaseApplLayer::populateWSM(wsm);
//
//
//    wsm->setNodeMobilityCoord(mobility->getPositionAt(simTime()));
//    wsm->setIsBeacon(true); // Set your beacon flag here
//    wsm->setTimestamp(simTime());
////    packetsSent++;
//}


