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

#include "veins/modules/messages/BaseFrame1609_4_m.h"
#include "veins/modules/messages/DemoSafetyMessage_m.h"
#include "veins/base/phyLayer/PhyToMacControlInfo.h"


#include "veins/base/modules/BaseMobility.h"
#include "veins/base/utils/FindModule.h"
#include <omnetpp.h>

// Standard library includes
#include <string>
#include <vector>
#include <stdexcept> // For std::runtime_error used in MACAddress constructor

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
void TraCIDemoRSU11p::handleMessage(cMessage* msg)
{
    EV_INFO << "------------------------------handlemessage---------"<<endl;


//        // Check if it's a wireless message from a vehicle
        if (msg->getArrivalGate() == gate("lowerLayerIn")) {
            EV_ERROR << "--> RSU DEBUG: Received message from wireless NIC. Class Name is: " << msg->getClassName() << endl;
            if(TraCIDemo11pMessage* v2v_msg = dynamic_cast<TraCIDemo11pMessage*>(msg)){


                EV_INFO << "RSU received TraCIDemo11pMessage from vehicle, forwarding to controller." << endl;

                Coord senderPos = v2v_msg->getNodeMobilityCoord();

                L3Address controllerIp = L3AddressResolver().resolve("open_flow_controller1");
                int controllerPort = par("controllerPort");

                cPacket* payload = v2v_msg->dup();
                EV_INFO << "RSU received message from vehicle " << v2v_msg->getSenderAddress()
                                << " at position (" << senderPos.x << "," << senderPos.y
                                << "), forwarding to controller." << endl;

                udpSocket.sendTo(payload, controllerIp, controllerPort);

                    // We are now done with the original incoming wireless message.
                delete v2v_msg;
            }
            else if (DemoSafetyMessage* bsm = dynamic_cast<DemoSafetyMessage*>(msg)){
                EV_INFO << "RSU received a DemoSafetyMessage (Beacon) from vehicle, forwarding to controller." << endl;
                // The BSM is a BaseFrame1609_4, which holds the sender's address and position.
                    BaseFrame1609_4* frame = check_and_cast<BaseFrame1609_4*>(bsm);

                    TraCIDemo11pMessage* msgForController = new TraCIDemo11pMessage();
                    LAddress::L2Type senderAddress;


                    cObject* ctrlInfo = bsm->getControlInfo();
                    PhyToMacControlInfo* phyCtrlInfo = dynamic_cast<PhyToMacControlInfo*>(ctrlInfo);

                    if (phyCtrlInfo) {
                        senderAddress = phyCtrlInfo->getSourceAddress();
                    }
                    else {
                                               // If control info is missing, we can't process the beacon
                        EV_ERROR << "RSU received beacon without PhyToMacControlInfo. Cannot process." << endl;
                        delete bsm;
                    //                           return; // Stop processing this message
                    }
                    // --- FINAL FIX ---
                    // Get the sender's address and position from the parent frame object.
                    msgForController->setSenderAddress(senderAddress);
                    msgForController->setNodeMobilityCoord(bsm->getSenderPos());


                    msgForController->setDemoData("Beacon Forwarded by RSU");

                    L3Address controllerIp = L3AddressResolver().resolve("open_flow_controller1");
                    int controllerPort = par("controllerPort");

                    udpSocket.sendTo(msgForController, controllerIp, controllerPort);

                delete bsm;
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
